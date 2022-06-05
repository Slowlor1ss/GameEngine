#pragma once
#include "Component.h"
#include "Utils.hpp"

namespace biggin
{
	class SpriteRenderComponent;
	class GameTime;
}

namespace burgerTime
{
	class PepperComponent final : public biggin::Component
	{
	public:
		explicit PepperComponent(biggin::GameObject* go);
		~PepperComponent() override = default;

		PepperComponent(const PepperComponent& other) = delete;
		PepperComponent(PepperComponent&& other) noexcept = delete;
		PepperComponent& operator=(const PepperComponent& other) = delete;
		PepperComponent& operator=(PepperComponent&& other) noexcept = delete;

		void Initialize(biggin::GameObject*) override;
		void Update() override;

	private:
		biggin::SpriteRenderComponent* m_pSpriteComp{ nullptr };

		utils::DelayedCallback m_DelayedRemove{};
		biggin::GameTime& m_GameTimeRef;
	};
}

