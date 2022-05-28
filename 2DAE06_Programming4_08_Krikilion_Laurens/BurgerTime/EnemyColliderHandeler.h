#pragma once
#include <memory>
#include <string>
#include <vector>
#include "Component.h"
#include "Observer.h"

//will catch overlapping events with player and burger
//if pepper -> get stunned, call stunned on movement comp
//if player and not stunned by pepper -> kill player;
//if burger ->
//check if burger is falling -> die
//else
//subscribe to burger events in case of burger falling event just die to keep it simple ~~disable enemy~~
//also add a tag to this class to know what enemy is is

//IN BURGER
//check if a enemy is overlapping and burger isn't falling -> add enemy overlapping count (from the enemy collider handeler)
//if enemy stops overlapping and burger isn't falling --overlapping count
//when falling fall as many levels as the enemy overlapping count

//ENEMY SPAWNER
//checks how many enemies have spawned and recives events when an enemy has died
//when an enemy died it is responsible for deleting it and --enemy count
//also keep track of what type the enemy is

//Implement score system
//Keep highscore
//Find out when you get extra pepper?

//Update enemy ai to target one of the 2 players

class EnemyMovement;

class EnemyColliderHandeler final : public biggin::Component, public biggin::Observer
{
public:
	EnemyColliderHandeler(biggin::GameObject* go);
	~EnemyColliderHandeler() = default;

	EnemyColliderHandeler(const EnemyColliderHandeler& other) = delete;
	EnemyColliderHandeler(EnemyColliderHandeler&& other) noexcept = delete;
	EnemyColliderHandeler& operator=(const EnemyColliderHandeler& other) = delete;
	EnemyColliderHandeler& operator=(EnemyColliderHandeler&& other) noexcept = delete;

	void Initialize(biggin::GameObject*) override;
	void OnNotify(Component* entity, const std::string& event) override;
	void Update() override;
	void FixedUpdate() override;

private:
	bool m_Stunned{false};
	bool m_IsAlive{true};
	std::vector<std::shared_ptr<biggin::GameObject>> m_PlayerRef{ nullptr };
	EnemyMovement* m_MovementRef{nullptr};
};

