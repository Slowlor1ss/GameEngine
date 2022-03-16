#include "BigginPCH.h"
#include "HealthComponent.h"
#include "GameObject.h"
#include "Subject.h"

using namespace biggin;

HealthComponent::HealthComponent(GameObject* go, int initialLives) : Component(go)
	,m_LivesLeft(initialLives)
	,m_pNotifier(go->GetComponent<Subject>())
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
