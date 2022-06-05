#pragma once
#pragma warning(push, 0)
#include <glm/glm.hpp>
#pragma warning(pop)
#include "Component.h"
#include "PossessGameObjectComponent.h"
#include "Subject.h"

//add pepper shooter to player game object
//implement the shoot function just spawn a pepperComponent (make sure the collider has a correct name see enemy for correct name)
//test the pepper duh...
//add a pepper ui element and visualize the pepper count (also add a pepper changed function when the value gets updated)
//set correct anaimation when throwing pepper
//add a pepper command and bind it to the correct buttons

namespace character
{
	enum class MoveDirection;
}

namespace burgerTime
{
	class PepperShooter final : public biggin::Component
	{
	public:
		PepperShooter(biggin::GameObject* go, const std::vector<biggin::Observer*>& observers, int amntPepper = 5);

		void Start() override;

		void SetAmntPepper(int amntPepper) { m_AmntPepper = amntPepper; m_pNotifier->notify(this, "PepperChanged"); }
		int GetAmntPepper() const { return m_AmntPepper; }
		void IncreasePepper(int amnt = 1) { m_AmntPepper += amnt; m_pNotifier->notify(this, "PepperChanged"); }

		bool Shoot(biggin::PossessGameObjectComponent::MoveDirection dir);

	private:
		int m_AmntPepper;
		biggin::Subject* m_pNotifier;
	};
}

