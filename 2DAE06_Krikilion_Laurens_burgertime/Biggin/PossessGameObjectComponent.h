#pragma once
#include <unordered_map>
#include "Component.h"

namespace biggin
{
	class SpriteRenderComponent;

	class PossessGameObjectComponent final : public biggin::Component
	{
	public:
		enum class MoveDirection { Left, Right, Up, Down, None };

		PossessGameObjectComponent(GameObject* go, float maxSpeed = 50);
		~PossessGameObjectComponent() override = default;

		PossessGameObjectComponent(const PossessGameObjectComponent& other) = delete;
		PossessGameObjectComponent(PossessGameObjectComponent&& other) noexcept = delete;
		PossessGameObjectComponent& operator=(const PossessGameObjectComponent& other) = delete;
		PossessGameObjectComponent& operator=(PossessGameObjectComponent&& other) noexcept = delete;

		void Initialize(GameObject*) override;
		void FixedUpdate() override;

		void MapAnimToDir(MoveDirection dir, int animIdx) { m_DirAnimMap[dir] = animIdx; }

		void SetVelocity(const glm::vec2& velocity);
		void SetSpeed(float maxSpeed) { m_MaxSpeed = maxSpeed; }
		float GetSpeed() const { return m_MaxSpeed; }

		void SetDisable(bool disable) { m_IsDisabled = disable; }
		bool IsDisabled() const { return m_IsDisabled; }

		MoveDirection GetCurrMovementDir() const { return m_CurrMovementDir; }
		//should never be none
		MoveDirection GetLastDirection() const { return m_LastDirection; }

	private:
		void UpdateMovementDirectionState();

		GameObject* m_pGameObjectRef;

		std::unordered_map<MoveDirection, int> m_DirAnimMap{{ MoveDirection::Left, -1 }, { MoveDirection::Right, -1 }, { MoveDirection::Up, -1 }, { MoveDirection::Down, -1 }, { MoveDirection::None, -1 }};
		MoveDirection m_CurrMovementDir{ MoveDirection::None };
		//should never be none
		MoveDirection m_LastDirection{ MoveDirection::Down };
		SpriteRenderComponent* m_pSpriteComp{nullptr};

		float m_MaxSpeed;
		glm::vec2 m_Velocity{};

		bool m_IsDisabled{ false };
	};
}
