#pragma once
#include <memory>
#include <string>
#include <vector>
#include "Component.h"
#include "Observer.h"
#include "Utils.hpp"

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

//use more namespaces

class Burger;

namespace biggin
{
	class PossessGameObjectComponent;
	class GameTime;
	class SpriteRenderComponent;
	class Subject;
	class BoxCollider2d;
}


namespace enemy
{
	enum class EnemyType;
	class EnemyMovement;

	class EnemyColliderHandeler final : public biggin::Component, public biggin::Observer
	{
	public:
		EnemyColliderHandeler(biggin::GameObject* go, EnemyType type, const std::vector<Observer*>& observers, bool isPossessed = false);
		~EnemyColliderHandeler();

		EnemyColliderHandeler(const EnemyColliderHandeler& other) = delete;
		EnemyColliderHandeler(EnemyColliderHandeler&& other) noexcept = delete;
		EnemyColliderHandeler& operator=(const EnemyColliderHandeler& other) = delete;
		EnemyColliderHandeler& operator=(EnemyColliderHandeler&& other) noexcept = delete;

		void Initialize(biggin::GameObject*) override;
		void Update() override;
		void OnNotify(Component* entity, const std::string& event) override;

		bool GetIsPossessed() const { return m_IsPossessed; }
		bool IsStunned() const { return m_Stunned; }
		EnemyType GetEnemyType() const { return m_EnemyType; }

	private:
		void Die(const biggin::GameObject* playerGo);
		void UpdateScoreOnDeath(const biggin::GameObject* playerGo) const;
		void HandleEnemyPlayerBeginContact(const biggin::BoxCollider2d* otherColider);
		void HandleEnemyBurgerBeginContact(const biggin::BoxCollider2d* otherColider);
		void HandleEnemyBurgerEndContact(const biggin::BoxCollider2d* otherColider);
		bool IsBurgerFalling(const Burger* burger) const;

		EnemyType m_EnemyType{};
		bool m_Stunned{false};
		bool m_IsAlive{true};
		std::vector<std::shared_ptr<biggin::GameObject>> m_PlayerRef{ nullptr };
		EnemyMovement* m_MovementRef{nullptr};
		biggin::GameObject* m_BurgerGameObjectRef{nullptr};

		biggin::SpriteRenderComponent* m_pSpriteComp{ nullptr };
		utils::DelayedCallback m_DelayedResetStunned{};
		biggin::GameTime& m_GameTimeRef;

		int m_AmntColliding{};

		bool m_IsPossessed;
		biggin::PossessGameObjectComponent* m_pMovementComp{nullptr	};

		biggin::Subject* m_pNotifier{ nullptr };
	};
}

