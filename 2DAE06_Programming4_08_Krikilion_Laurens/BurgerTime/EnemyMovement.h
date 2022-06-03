#pragma once
#include <memory>
#include <string>
#include <vector>
#include "Component.h"
#include "Observer.h"
#include "Utils.hpp"


namespace biggin
{
	class GameTime;
	class SpriteRenderComponent;
}

class EnemyMovement final : public biggin::Component, public biggin::Observer
{
public:
	enum class movementDirection
	{
		movingUp,
		movingDown,
		movingLeft,
		movingRight,
		none,
	};
	enum class AnimationState { runHorizontal, runVertical, peppered, die };

	EnemyMovement(biggin::GameObject* go, float velocity);
	~EnemyMovement() override = default;

	EnemyMovement(const EnemyMovement& other) = delete;
	EnemyMovement(EnemyMovement&& other) noexcept = delete;
	EnemyMovement& operator=(const EnemyMovement& other) = delete;
	EnemyMovement& operator=(EnemyMovement&& other) noexcept = delete;

	void Initialize(biggin::GameObject*) override;
	void OnNotify(Component* entity, const std::string& event) override;
	void HandleHitWall(const glm::vec2& playerPos, const glm::vec2& pos, movementDirection wallDir);
	void Update() override;
	void FixedUpdate() override;

	//void Disable();
	void Peppered(float time = 3.f);
	void Die();

private:
	bool CheckAndFixStuck();
	void SetDirection(movementDirection newDir);
	//bool FixStuckMovingUp();
	//bool FixStuckMovingDown();
	//bool FixStuckMovingLeft();
	//bool FixStuckMovingRight();

	bool IsMovingSideways() const { return m_CurrentDirection == movementDirection::movingLeft || m_CurrentDirection == movementDirection::movingRight; }

	std::shared_ptr<biggin::GameObject> m_PlayerRef{ nullptr };

	int m_AmntCollidingTop{ 0 };
	int m_AmntCollidingBottom{ 0 };
	int m_AmntCollidingLeft{ 0 };
	int m_AmntCollidingRight{ 0 };

	float m_CooldownCounter{};
	float m_Cooldown{0.5f};

	movementDirection m_CurrentDirection;

	float m_Velocity{50};
	bool m_Disabled{false};
	bool m_Dead{false};

	biggin::SpriteRenderComponent* m_pSpriteComp{nullptr};

	utils::DelayedCallback m_DelayedResetDisabled{};
	biggin::GameTime& m_GameTimeRef;

	static constexpr float m_PlayerPosMargin{ 25 };
};

