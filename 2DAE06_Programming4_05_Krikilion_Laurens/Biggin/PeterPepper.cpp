#include "BigginPCH.h"
#include "PeterPepper.h"
#include "GameObject.h"
#include "HealthComponent.h"

character::PeterPepper::PeterPepper(biggin::GameObject* go) : Component(go)
	,m_pHealth(go->GetComponent<biggin::HealthComponent>())
	,m_IsDead(false)
{
}

void character::PeterPepper::Update()
{
	if (m_IsDead)
		return;

	//Do peter pepper stuff
}

void character::PeterPepper::Damage()
{
	m_pHealth->Damage();
	m_IsDead = true;
}
