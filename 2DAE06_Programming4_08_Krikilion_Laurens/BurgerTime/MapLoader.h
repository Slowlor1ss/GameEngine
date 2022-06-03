#pragma once
#include <map>
#include <memory>
#include <string>
#include <vector>
#include "BoxCollider2d.h"
#include "Burger.h"
#include "Component.h"
#include "EnemySpawner.h"
#include "Observer.h"
#include "Subject.h"

class EnemySpawner;
enum class BurgerIngredients;

namespace character
{
	class PeterPepper;
}

namespace biggin
{
	class RenderComponent;
	class GameObject;
}

namespace burgerTime
{
	class MapLoader final : public biggin::Component, public biggin::Observer
	{
		enum class MapValues : char
		{
			//Map
			Ladder = 'H',

			ColliderBegin = 'b',
			ColliderEnd = 'e',
			ColliderBeginBig = 'B',
			ColliderEndBig = 'E',

			//colliders that should block the player but not items
			//and are bigger than the normal colliders to fill up the necessary space
			ExtraColliderStart = 'S',
			ExtraColliderQuit = 'Q',

			//used for catching the burgers
			CatcherBegin = 'C',
			CatcherEnd = 'A',

			//Spawner that spawn's Mr. Hot Dog, Mr. Pickle, and Mr. Egg
			EnemySpawner = 'M',

			//Items
			BurgerTop = '1',
			Lettuce = '2',
			Meat = '3',
			BurgerBottom = '4',
			Cheese = '5',
			Tomato = '6',

			Pepper = 'P',

			Player = 'X'
		};

	public:
		MapLoader(biggin::GameObject* go, const std::vector<Observer*>& observers = {});
		~MapLoader() override;

		MapLoader(const MapLoader& other) = delete;
		MapLoader(MapLoader&& other) noexcept = delete;
		MapLoader& operator=(const MapLoader& other) = delete;
		MapLoader& operator=(MapLoader&& other) noexcept = delete;

		void Initialize(biggin::GameObject*) override;
		void Start() override;
		void OnNotify(Component* entity, const std::string& event) override;
		void Update() override;

		void ResetLevel();
		void LoadNext();

		static constexpr float GetGridSize() { return m_GridCellSize; }

	private:
		void ReadJsonLevelFile(const std::string& file);
		void CreateMapSideColliders();
		void LoadMap(const std::string& file);
		void ReadLevelFile(const std::string& file);
		void ReadItemsFile(const std::string& file);
		void ProcessLineMapFile(const std::string& line);
		void ProcessLineItemsFile(const std::string& line);

		void MakeCollider(int idx, int colliderBeginPos, int sizeMultiplier = 1, unsigned short collisionGroup = 8, std::string tag = "Platform") const;
		void SpawnBurgerPart(int& idx, BurgerIngredients ingredent);
		void SpawnEnemySpawner(int idx, int mapMaxWidthIdx) const;

		int m_CurrentLevelIdx;

		std::vector<std::string> m_LevelPaths;
		std::vector<std::string> m_ImagePaths;


		std::vector<EnemySpawner::EnemySettings> m_EnemySettingsPerLevel{};
		int m_LineNumber{0};
		int m_MapMaxWidthIdx{0};

		int m_AmntBurgerParts{};
		int m_AmntLandedParts{};

		bool m_LoadNextLevel;

		biggin::GameObject* m_GameObjectRef{ nullptr };

		std::vector<std::shared_ptr<biggin::GameObject>> m_PlayerRef{};
		glm::vec2 m_PlayerSpawnLocation{};
		std::vector<utils::DelayedCallback> m_DelayedRespawns{};
		GameTime& m_GameTimeRef;

		EnemySpawner* m_EnemySpawnerRef{ nullptr };
		biggin::RenderComponent* m_BackgroundImgRef{ nullptr };
		biggin::Subject* m_pNotifier;

		static constexpr float m_GridCellSize{ 16 };//size of one cell is 16x16
		//static constexpr unsigned short m_MapCollisionGroups{ biggin::BoxCollider2d::CollisionGroup::Group3 | Burger::burgerIgnoreGroup };

	public:
		enum mapCollisionGroup : unsigned short
		{
			platformGroup = biggin::BoxCollider2d::CollisionGroup::Group3,
			mapGroup = platformGroup | static_cast<int>(Burger::burgerIgnoreGroup) //all collision groups that get used in the map
		};

	};

	//44. Prefer writing nonmember nonfriend functions.
	static void BurgerPrefab(BurgerIngredients ingredient, glm::vec2 pos, const std::vector<biggin::Observer*>& observers = {});
	//static void EnemySpawnerPrefab(glm::vec2 pos);
}