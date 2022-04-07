#pragma once
#include "Component.h"
#include "Observer.h"

namespace biggin
{
	class HealthComponent;
}

namespace character
{
	class PeterPepper final : public biggin::Component, public biggin::Observer
	{
	public:
		PeterPepper(biggin::GameObject* go);

		void Update() override;

		void Damage();
		void OnNotify(const Component* entity, const std::string& event) override;

	private:
		bool m_IsDead;

		biggin::HealthComponent* m_pHealth;
	};
}
