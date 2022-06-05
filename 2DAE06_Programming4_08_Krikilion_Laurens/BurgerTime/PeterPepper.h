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
	//enum class MoveDirection { Left, Right, Up, Down, None};
	class PeterPepper final : public biggin::Component, public biggin::Observer
	{

	public:
		PeterPepper(biggin::GameObject* go, float movementSpeed = 1);
		~PeterPepper() override = default;

		PeterPepper(const PeterPepper& other) = delete;
		PeterPepper(PeterPepper&& other) noexcept = delete;
		PeterPepper& operator=(const PeterPepper& other) = delete;
		PeterPepper& operator=(PeterPepper&& other) noexcept = delete;

		void Initialize(biggin::GameObject* go) override;
		void Update() override;
		void FixedUpdate() override;

		void SetVelocity(const glm::vec2& velocity);
		void RespawnPlayer(const glm::vec2& pos);
		void Damage();
		bool IsDead() const { return m_IsDead; }
		float GetSpeed() const { return m_Speed; }
		int GetHealth() const;
		void IncreaseHealth() const { if(m_pHealthComp) m_pHealthComp->Heal(); };

		void ShootPepper();

		void OnNotify(Component* entity, const std::string& event) override;

	private:
		//void UpdateAnimationState();
		//void UpdateMovementDirectionState();
		bool m_IsDead;
		bool m_IsShooting;
		utils::DelayedCallback m_DelayedStopShooting{};
		biggin::GameTime& m_GameTimeRef;
		float m_Speed;
		glm::vec2 m_Velocity{};
		inline static int m_AmntPlayers{0};
		int m_PlayerIndex;
		//MoveDirection m_CurrMovementDir{MoveDirection::None};
		//MoveDirection m_LastMovementDir{MoveDirection::Down};
		//AnimationState m_CurrAnimState{AnimationState::Idle};
		//bool m_VerticalMovDisabled{ false };
		//bool m_HorizontalMovDisabled{ false };

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
