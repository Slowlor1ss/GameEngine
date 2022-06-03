#include "PepperShooter.h"

burgerTime::PepperShooter::PepperShooter(biggin::GameObject* go, int amntPepper)
	: Component(go)
	, m_AmntPepper(amntPepper)
{
}

bool burgerTime::PepperShooter::Shoot(glm::vec2 pos)
{
	if (m_AmntPepper <= 0)
		return false;

	//spawn pepper
	--m_AmntPepper;

	return true;
}
