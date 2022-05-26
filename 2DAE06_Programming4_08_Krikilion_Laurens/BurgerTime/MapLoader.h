#pragma once
#include <map>
#include <memory>
#include <string>
#include <vector>
#include "Component.h"
#include "Observer.h"
#include "Subject.h"

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

class MapLoader final : public biggin::Component, public biggin::Observer
{
	//TODO: add extra collider
	//TODO: add player location
	enum class MapValues : char
	{
		//Map
		Ladder = 'H',
		ColliderBegin = 'B',
		ColliderEnd = 'E',
		ExtraColliderStart = 'S', //colliders that should block the player but not items
		ExtraColliderQuit = 'Q',
		CatcherBegin = 'C',
		CatcherEnd = 'A',
		//Items
		BurgerTop = '1',
		Lettuce = '2',
		Meat = '3',
		BurgerBottom = '4',
		Pepper = 'P',

		Player = 'X'
	};

public:
	MapLoader(biggin::GameObject* go, const std::vector<Observer*>& observers = {});
	~MapLoader() override = default;

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
	void LoadMap(const std::string& file);
	void ReadLevelFile(const std::string& file);
	void ReadItemsFile(const std::string& file);
	void ProcessLineMapFile(const std::string& line) const;
	void MakeCollider(int i, int colliderBeginPos, unsigned short collisionGroup = 1, std::string tag = "Platform") const;
	void ProcessLineItemsFile(const std::string& line);

	void SpawnBurgerPart(int& i, BurgerIngredients ingredent);

	int m_CurrentLevelIdx;

	std::vector<std::string> m_LevelPaths;
	std::vector<std::string> m_ImagePaths;
	int m_LineNumber{0};

	int m_AmntBurgerParts{};
	int m_AmntLandedParts{};

	bool m_LoadNextLevel;

	biggin::GameObject* m_GameObjectRef{ nullptr };
	std::vector<std::shared_ptr<biggin::GameObject>> m_PlayerRef{ nullptr };
	biggin::RenderComponent* m_BackgroundImgRef{ nullptr };
	biggin::Subject* m_pNotifier;

	static constexpr float m_GridCellSize{ 16 };//size of one cell is 16x16

};