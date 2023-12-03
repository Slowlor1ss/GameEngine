#pragma once
#include "BoxCollider2d.h"
#include "Component.h"
#pragma warning(push, 0)
#include <glm/glm.hpp>
#pragma warning(pop)
#include <string>

namespace burgerTime
{
	class PepperShooter;
}

namespace biggin
{
	class PossessGameObjectComponent;
	class GameTime;
	class BoxCollider2d;
	class SpriteRenderComponent;
	class HealthComponent;
}

namespace character
{
	enum class AnimationState { Idle, RunHorizontal, RunVertical, PepperHorizontal, PepperVertical, Die };

	class PeterPepper final : public biggin::Component
	{
	public:
		PeterPepper(biggin::GameObject* go);

		void Initialize(biggin::GameObject* go) override;
		void Update() override;

	private:
		biggin::GameTime& m_GameTimeRef;

		biggin::GameObject* m_pGameObjectRef;

		biggin::SpriteRenderComponent* m_pSpriteComp;
		biggin::PossessGameObjectComponent* m_pMovementComp;

		biggin::Subject* m_pNotifier{ nullptr };

	public:
		enum PlayerCollisionGroup : unsigned short
		{
			playerIgnoreGroup = biggin::BoxCollider2d::CollisionGroup::Group4,
			playerCollisionGroup = biggin::BoxCollider2d::CollisionGroup::Group5,
		};
	};
}
