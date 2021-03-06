#pragma once
#include <memory>
#include <string>
#include <vector>
#pragma warning(push, 0)
#include <glm/glm.hpp>
#pragma warning (pop)
#include "Component.h"
#include "Observer.h"
#include "Utils.hpp"

namespace biggin
{
	class GameTime;
}

namespace enemy
{
	class EnemyColliderHandeler;

	enum class EnemyType
	{
		HotDog,
		Pickle,
		Egg
	};

	class EnemySpawner final : public biggin::Component, public biggin::Observer
	{
	public:
		struct EnemySettings
		{
			int maxHotDogs{ 0 };
			int maxEggs{ 0 };
			int maxPickles{ 0 };
			float velocity{ 50 };
		};

		explicit EnemySpawner(biggin::GameObject* go, bool hasPossessedEnemy = false);

		void Initialize(biggin::GameObject*) override;
		void Update() override; 
		void OnNotify(Component* entity, const std::string& event) override;
		void RenderDebug() override;

		void ResetEnemyData();
		void FullReset();

		void AddSpawnLocation(const glm::vec2& pos) { m_SpawnPositions.emplace_back(pos); m_FreeSpawnPositions.emplace_back(pos); }
		void SetMaxEnemies(EnemySettings settings) { m_Settings = settings; }

	private:
		void SpawnEnemy(EnemyType type, glm::vec2 pos);
		void SpawnPossessedEnemy(glm::vec2 pos);
		void PickEnemyAndLocation();
		void SpawnEnemyAtRandLocDelayed(EnemyType type);
		void SpawnPossessedEnemyAtRandLocDelayed();

		biggin::GameObject* m_GameObjectRef{ nullptr };
		int m_AmntHotDogs{};
		int m_AmntEggs{};
		int m_AmntPickle{};
		bool m_HasPossessedHotDog{ false };
		bool m_SpawnPossessedHotDog{ false };
		EnemySettings m_Settings{};
		std::vector<glm::vec2> m_SpawnPositions{};
		std::vector<glm::vec2> m_FreeSpawnPositions{};
		//std::vector<EnemyColliderHandeler*> m_Enemies{};

		utils::DelayedCallback m_DelayedSpawn{};
		utils::DelayedCallback m_DelayedSpawn2{};
		biggin::GameTime& m_GameTimeRef;
	};
}

