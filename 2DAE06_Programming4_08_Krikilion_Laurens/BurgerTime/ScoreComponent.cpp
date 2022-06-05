#include "BigginPCH.h"
#include "ScoreComponent.h"
#include <fstream>
#include "GameObject.h"
#include "ResourceManager.h"
#include "Subject.h"

using namespace burgerTime;

ScoreComponent::ScoreComponent(biggin::GameObject* go, const std::vector<biggin::Observer*>& observers )
	: Component(go)
	, m_HighScoreFilePath(biggin::ResourceManager::GetInstance().GetDataPath() + "HighScore.txt")
	, m_pNotifier(go->GetComponent<biggin::Subject>())
{
	for (const auto observer : observers)
		m_pNotifier->AddObserver(observer);
}

void ScoreComponent::Start()
{
	//Updating the initial score
	m_pNotifier->notify(this, "ScoreChanged");
	m_pNotifier->notify(this, "HighScoreChanged");
}

int ScoreComponent::GetScore() const
{
	return m_Score; //TODO: change this
}

void ScoreComponent::IncreaseScore(int amount)
{
	m_Score += amount;
	m_pNotifier->notify(this, "ScoreChanged");
}

void burgerTime::ScoreComponent::SaveHighScore()
{
	std::multiset<int, std::greater<int>> scores{};
	std::fstream f;
	f.open(m_HighScoreFilePath, std::ifstream::in);
	if (f.is_open()) {
		int a;
		while (f >> a) {
			scores.insert(a);
		}
		f.close();
	}

	scores.insert(m_Score);

	f.open(m_HighScoreFilePath, std::ifstream::out | std::ifstream::trunc);
	if (f.is_open()) {
		for (const int score : scores)
		{
			f << score << '\n';
		}
		f.close();
	}

	m_pNotifier->notify(this, "HighScoreChanged");
}

std::multiset<int, std::greater<int>> burgerTime::ScoreComponent::GetHighScores() const
{
	std::multiset<int, std::greater<int>> scores{};
	std::ifstream f;
	f.open(m_HighScoreFilePath, std::ifstream::in);

	if (f.is_open()) {
		int a;
		while (f >> a) {
			scores.insert(a);
		}
		f.close();
	}

	//only keep 5 highest scores
	if (scores.size() > 5)
		scores.erase( std::next(scores.begin(), 4), scores.end());

	return scores;
}
