#include "BigginPCH.h"
#include "HealthComponent.h"
#include "GameObject.h"
#include "Subject.h"

using namespace biggin;

HealthComponent::HealthComponent(GameObject* go, int initialLives, const std::vector<Observer*>& observers) : Component(go)
	,m_LivesLeft(initialLives)
	,m_pNotifier(go->GetComponent<Subject>())
{
	for (const auto observer : observers)
		m_pNotifier->AddObserver(observer);
}

void biggin::HealthComponent::Start()
{
	//Updating the initial health
	m_pNotifier->notify(this, "HealthChanged");
}

int HealthComponent::GetLives() const
{
	return  m_LivesLeft;
}

void HealthComponent::Damage()
{
	--m_LivesLeft;
	m_pNotifier->notify(this, "HealthChanged");
}
