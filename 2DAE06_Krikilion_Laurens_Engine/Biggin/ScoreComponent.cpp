#include "BigginPCH.h"
#include "ScoreComponent.h"
#include "GameObject.h"
#include "StatsAndAchievements.h"
#include "Subject.h"

biggin::ScoreComponent::ScoreComponent(GameObject* go) : Component(go)
	, m_pNotifier(go->GetComponent<Subject>())
{
}

void biggin::ScoreComponent::Start()
{
	//Updating the initial score
	m_pNotifier->notify(this, "ScoreChanged");
	CSteamAchievements::GetInstance().AddObserver(m_pNotifier);
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
