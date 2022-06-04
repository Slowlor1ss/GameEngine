#pragma once
#include "Component.h"

namespace biggin
{
	class Observer;
	class Subject;

	class HealthComponent final : public biggin::Component
	{
	public:
		explicit HealthComponent(GameObject* go, int initialLives = 4, const std::vector<Observer*>& observers = {});

		void Start() override;

		int GetLives() const;
		void Damage();
		void Heal();

	private:
		int m_LivesLeft;

		Subject* m_pNotifier;
	};
}

