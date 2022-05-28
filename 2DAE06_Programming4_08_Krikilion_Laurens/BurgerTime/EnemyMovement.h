#pragma once
#include <memory>
#include <string>
#include <vector>
#include "Component.h"
#include "Observer.h"
#include "Utils.hpp"

class GameTime;

namespace biggin
{
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

	EnemyMovement(biggin::GameObject* go, movementDirection movDir);
	~EnemyMovement() override = default;

	EnemyMovement(const EnemyMovement& other) = delete;
	EnemyMovement(EnemyMovement&& other) noexcept = delete;
	EnemyMovement& operator=(const EnemyMovement& other) = delete;
	EnemyMovement& operator=(EnemyMovement&& other) noexcept = delete;

	void Initialize(biggin::GameObject*) override;
	void OnNotify(Component* entity, const std::string& event) override;
	void Update() override;
	void FixedUpdate() override;

	void Disable();
	void Peppered(float time = 3.f);
	void Die();

private:
	bool CheckAndFixStuck();
	bool FixStuckMovingUp();
	bool FixStuckMovingDown();
	bool FixStuckMovingLeft();
	bool FixStuckMovingRight();

	bool IsMovingSideways() const { return m_CurrentDirection == movementDirection::movingLeft || m_CurrentDirection == movementDirection::movingRight; }

	std::shared_ptr<biggin::GameObject> m_PlayerRef{ nullptr };

	int m_AmntCollidingTop{ 0 };
	int m_AmntCollidingBottom{ 0 };
	int m_AmntCollidingLeft{ 0 };
	int m_AmntCollidingRight{ 0 };

	movementDirection m_CurrentDirection;
	//used to make sure the enemy doesn't quickly move to the side and then back up or down
	//only stays available for a short time
	movementDirection m_OldDirection{movementDirection::none}; 
	float m_Velocity{50};
	bool m_Disabled{false};

	biggin::SpriteRenderComponent* m_pSpriteComp{nullptr};
	utils::DelayedCallback m_DelayedResetOldDir;
	utils::DelayedCallback m_DelayedResetDisabled;
	GameTime& m_GameTimeRef;

	static constexpr float m_PlayerPosMargin{ 2 };
};

