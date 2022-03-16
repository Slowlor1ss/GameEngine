#pragma once
#include "Component.h"

namespace biggin
{
	class HealthComponent;
}

namespace character
{
	class PeterPepper final : public biggin::Component
	{
	public:
		PeterPepper(biggin::GameObject* go);
		
		void Start() override {};
		void Update() override;
		void FixedUpdate() override {};
		void Render() const override {};
		void RenderUi() override {};

		void Damage();

	private:
		bool m_IsDead;

		std::shared_ptr<biggin::HealthComponent> m_pHealth;
	};
}
