#pragma once
#pragma warning(push, 0)
#include <glm/glm.hpp>
#pragma warning(pop)
#include "Component.h"

//add pepper shooter to player game object
//implement the shoot function just spawn a pepperComponent (make sure the collider has a correct name see enemy for correct name)
//test the pepper duh...
//add a pepper ui element and visualize the pepper count (also add a pepper changed function when the value gets updated)

namespace burgerTime
{
	class PepperShooter final : public biggin::Component
	{
	public:
		PepperShooter(biggin::GameObject* go, int amntPepper = 5);
		~PepperShooter() override = default;

		PepperShooter(const PepperShooter& other) = delete;
		PepperShooter(PepperShooter&& other) noexcept = delete;
		PepperShooter& operator=(const PepperShooter& other) = delete;
		PepperShooter& operator=(PepperShooter&& other) noexcept = delete;

		void SetAmntPepper(int amntPepper) { m_AmntPepper = amntPepper; }
		void IncreasePepper(int amnt = 1) { m_AmntPepper += amnt; }
		void DecreasePepper(int amnt = 1) { m_AmntPepper -= amnt; }

		bool Shoot(glm::vec2 pos);

	private:
		int m_AmntPepper;
	};
}

