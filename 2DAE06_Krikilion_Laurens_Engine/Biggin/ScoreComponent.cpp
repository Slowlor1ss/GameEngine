#include "BigginPCH.h"
#include "ScoreComponent.h"
#include "Subject.h"

biggin::ScoreComponent::ScoreComponent(GameObject* go) : Component(go)
{
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
}
