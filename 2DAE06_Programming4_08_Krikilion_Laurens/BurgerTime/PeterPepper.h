#pragma once
#include "Component.h"
#include "Observer.h"

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
		//TODO: use state pattern
		enum class MoveDirection { Left, Right, Up, Down, None};

	public:
		PeterPepper(biggin::GameObject* go, float movementSpeed = 1);

		void Initialize(biggin::GameObject* go) override;
		void Update() override;
		void FixedUpdate() override;

		void AddVelocity(const glm::vec2& velocity) { m_Velocity += velocity; }
		void SetVelocity(const glm::vec2& velocity) { m_Velocity = velocity; }
		void SetPosition(const glm::vec2& pos) const;
		void Damage();
		bool IsDead() const { return m_IsDead; }
		float GetSpeed() const { return m_Speed; }

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
		AnimationState m_CurrAnimState{AnimationState::Idle};
		bool m_VerticalMovDisabled{ false };
		bool m_HorizontalMovDisabled{ false };

		biggin::GameObject* m_pGameObjectRef{ nullptr };

		biggin::HealthComponent* m_pHealthComp;
		biggin::SpriteRenderComponent* m_pSpriteComp;
	};
}
