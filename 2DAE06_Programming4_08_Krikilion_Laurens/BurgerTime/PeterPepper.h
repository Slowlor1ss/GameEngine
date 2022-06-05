#pragma once
#include "BoxCollider2d.h"
#include "Component.h"
#include "Observer.h"
#pragma warning(push, 0)
#include <glm/glm.hpp>
#include "HealthComponent.h"
#pragma warning(pop)

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

		void RespawnPlayer(const glm::vec2& pos);
		void Damage();
		bool IsDead() const { return m_IsDead; }
		int GetHealth() const;
		void IncreaseHealth() const { if(m_pHealthComp) m_pHealthComp->Heal(); };

		void ShootPepper();

	private:
		bool m_IsDead;
		bool m_IsShooting;
		utils::DelayedCallback m_DelayedStopShooting{};
		biggin::GameTime& m_GameTimeRef;

		biggin::GameObject* m_pGameObjectRef;

		biggin::HealthComponent* m_pHealthComp;
		burgerTime::PepperShooter* m_pPepperShooter;
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
