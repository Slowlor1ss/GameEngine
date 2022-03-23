#include "BigginPCH.h"
#include "ScoreComponent.h"
#include "GameObject.h"
#include "StatsAndAchievements.h"
#include "Subject.h"

biggin::ScoreComponent::ScoreComponent(GameObject* go, const std::vector<Observer*>& observers ) : Component(go)
	, m_pNotifier(go->GetComponent<Subject>())
{
	for (const auto observer : observers)
		m_pNotifier->AddObserver(observer);
}

void biggin::ScoreComponent::Start()
{
	//Updating the initial score
	m_pNotifier->notify(this, "ScoreChanged");
}

int biggin::ScoreComponent::GetScore() const
{
	return m_Score;
}

void biggin::ScoreComponent::IncreaseScore(int amount)
{
	m_Score += amount;
	m_pNotifier->notify(this, "ScoreChanged");
}
