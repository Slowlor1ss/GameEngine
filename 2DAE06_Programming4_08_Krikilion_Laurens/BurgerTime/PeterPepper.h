#pragma once
#include "BoxCollider2d.h"
#include "Component.h"
#include "Observer.h"
#pragma warning(push, 0)
#include <glm/glm.hpp>
#pragma warning(pop)

namespace biggin
{
	class BoxCollider2d;
	class SpriteRenderComponent;
	class HealthComponent;
}

namespace character
{
	//TODO: use state pattern
	enum class AnimationState { Idle, RunHorizontal, RunVertical, PepperHorizontal, PepperVertical, Die };
	class PeterPepper final : public biggin::Component, public biggin::Observer
	{
		enum class MoveDirection { Left, Right, Up, Down, None};

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

		void AddVelocity(const glm::vec2& velocity) { m_Velocity += velocity; }
		void SetVelocity(const glm::vec2& velocity) { m_Velocity = velocity; }
		void SetPosition(const glm::vec2& pos) const;
		void RespawnPlayer(const glm::vec2& pos);
		void Damage();
		bool IsDead() const { return m_IsDead; }
		float GetSpeed() const { return m_Speed; }
		int GetHealth() const;

		void ShootPepper();

		void OnNotify(Component* entity, const std::string& event) override;


	private:
		void UpdateAnimationState();
		void UpdateMovementDirectionState();
		bool m_IsDead;
		float m_Speed;
		glm::vec2 m_Velocity{};
		inline static int m_AmntPlayers{0};
		int m_PlayerIndex;
		MoveDirection m_CurrMovementDir{MoveDirection::None};
		MoveDirection m_LastMovementDir{MoveDirection::Down};
		AnimationState m_CurrAnimState{AnimationState::Idle};
		bool m_VerticalMovDisabled{ false };
		bool m_HorizontalMovDisabled{ false };

		biggin::GameObject* m_pGameObjectRef{ nullptr };

		biggin::HealthComponent* m_pHealthComp;
		biggin::SpriteRenderComponent* m_pSpriteComp;

		biggin::Subject* m_pNotifier{ nullptr };

	public:
		enum PlayerCollisionGroup : unsigned short
		{
			playerIgnoreGroup = biggin::BoxCollider2d::CollisionGroup::Group4,
			playerCollisionGroup = biggin::BoxCollider2d::CollisionGroup::Group5,
		};
	};
}
