#pragma once
#include <map>
#include "Component.h"

namespace character
{
	class PeterPepper;
}

namespace biggin
{
	class GameObject;
}

class MapLoader : public biggin::Component
{
	//TODO: add catcher the thing that catches the burger at the bottom so burgs know when to stop falling
	//TODO: add extra collider
	//TODO: add player location
	enum class MapValues : char
	{
		//Map
		Ladder = 'H',
		ColliderBegin = 'B',
		ColliderEnd = 'E',
		//Items
		BurgerTop = '1',
		Lettuce = '2',
		Meat = '3',
		BurgerBottom = '4',
		Pepper = 'P',

		Player = 'X'
	};

public:
	MapLoader(biggin::GameObject* go, const std::string& file, character::PeterPepper* player);
	~MapLoader() override = default;

	MapLoader(const MapLoader& other) = delete;
	MapLoader(MapLoader&& other) noexcept = delete;
	MapLoader& operator=(const MapLoader& other) = delete;
	MapLoader& operator=(MapLoader&& other) noexcept = delete;

	void Start() override;

private:
	void LoadMap(const std::string& file);
	void ReadLevelFile(const std::string& file);
	void ReadItemsFile(const std::string& file);
	void ProcessLineMapFile(const std::string& line) const;
	void ProcessLineItemsFile(const std::string& line) const;

	std::string m_File;
	int m_LineNumber{0};

	biggin::GameObject* m_GameObjectRef{ nullptr };
	character::PeterPepper* m_PlayerRef{ nullptr };

	static constexpr float m_GridCellSize{ 16 };//size of one cell is 16x16

};