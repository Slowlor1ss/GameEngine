#include "BigginPCH.h"
#include "ScoreComponent.h"
#include "GameObject.h"
#include "Subject.h"

using namespace burgerTime;

ScoreComponent::ScoreComponent(biggin::GameObject* go, const std::vector<biggin::Observer*>& observers )
	: Component(go)
	, m_pNotifier(go->GetComponent<biggin::Subject>())
{
	for (const auto observer : observers)
		m_pNotifier->AddObserver(observer);
}

void ScoreComponent::Start()
{
	//Updating the initial score
	m_pNotifier->notify(this, "ScoreChanged");
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
