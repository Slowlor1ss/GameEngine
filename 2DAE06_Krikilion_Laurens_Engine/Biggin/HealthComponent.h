#pragma once
#include "Component.h"

namespace biggin
{
	class Subject;

	class HealthComponent final : public biggin::Component
	{
	public:
		HealthComponent(biggin::GameObject* go, int initialLives = 4);

		void Start() override;

		int GetLives() const;
		void Damage();

	private:
		int m_LivesLeft;

		Subject* m_pNotifier;
	};
}

