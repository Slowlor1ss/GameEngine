#include "BigginPCH.h"
#include "MapLoader.h"
#include <fstream>
#include <Box2D/Dynamics/b2Body.h>
#include "Biggin.h"
#include "BoxCollider2d.h"
#include "Burger.h"
#include "GameLoader.h"
#include "GameObject.h"
#include "Logger.h"
#include "PeterPepper.h"

//Map loader works by reading a text file generated from a image with a small badly coded python script :D
//TODO: load background image
MapLoader::MapLoader(biggin::GameObject* go, const std::string& file, character::PeterPepper* player)
	: Component(go)
	, m_File(file)
	, m_GameObjectRef(go)
	, m_PlayerRef(player)
{
}

void MapLoader::Initialize(biggin::GameObject*)
{
	LoadMap(m_File);
}

void MapLoader::LoadMap(const std::string& file)
{
	//remove the extension if there is one
	const std::string rawName = file.substr(0, file.find_last_of("."));

    ReadLevelFile(rawName + ".txt");
    ReadItemsFile(rawName + "items.txt");
}

void MapLoader::ReadLevelFile(const std::string& file)
{
	//reset line number
	m_LineNumber = 0;

	std::ifstream f;
	f.open(file, std::ifstream::in);

	if (f.is_open()) {
		while (!f.eof()) {
			std::string line;
			std::getline(f, line);
			ProcessLineMapFile(line);
			++m_LineNumber;
		}
		f.close();
	}
	else {
		throw std::runtime_error("Could not find file: " + file);
	}
}

void MapLoader::ReadItemsFile(const std::string& file)
{
	//reset line number
	m_LineNumber = 0;

	std::ifstream f;
	f.open(file, std::ifstream::in);

	if (f.is_open()) {
		while (!f.eof()) {
			std::string line;
			std::getline(f, line);
			ProcessLineItemsFile(line);
			++m_LineNumber;
		}
		f.close();
	}
	else {
		throw std::runtime_error("Could not find file: " + file);
	}
}

void MapLoader::ProcessLineMapFile(const std::string& line) const
{
    int colliderBeginPos{ 0 };
    
	for (int i{0}; i < static_cast<int>(line.length()); ++i)
	{
		switch (static_cast<MapValues>(line[i]))
		{
		case MapValues::Ladder:
            break;
        case MapValues::ColliderBegin:
            colliderBeginPos = i;
            break;
        case MapValues::ColliderEnd:
	        {
			MakeCollider( i, colliderBeginPos);

	    //        colliderEndPos = i + 1;
	    //        float colliderWidth = m_GridCellSize * (colliderEndPos - colliderBeginPos);
	    //        float colliderHeight = m_GridCellSize * 0.5f;
	    //        glm::vec2 pos = { m_GridCellSize * colliderBeginPos, m_GridCellSize * m_LineNumber /*- colliderHeight */};

	    //        m_GameObjectRef->AddComponent(new biggin::BoxCollider2d(
		   //         m_GameObjectRef, {colliderWidth, colliderHeight}, true, b2_staticBody, 
					//{m_PlayerRef}, "DisableUp", pos, false));
	        }
            break;
		case MapValues::ExtraColliderStart:
			colliderBeginPos = i;
			break;
		case MapValues::ExtraColliderQuit:
			MakeCollider(i, colliderBeginPos, static_cast<unsigned short>(biggin::CollisionGroup::Group1));
			break;
		case MapValues::CatcherBegin:
			colliderBeginPos = i;
			break;
		case MapValues::CatcherEnd:
			MakeCollider(i, colliderBeginPos, static_cast<unsigned short>(biggin::CollisionGroup::None), "Catcher");
			break;
		case MapValues::Player:
			m_PlayerRef->SetPosition(glm::vec2{ m_GridCellSize * i, m_GridCellSize * m_LineNumber - m_GridCellSize * 0.5f } + m_GameObjectRef->GetLocalPosition());
			break;
        default: ;
		}
	}
}

void MapLoader::MakeCollider(int i, int colliderBeginPos, unsigned short collisionGroup, std::string tag) const
{
	const int colliderEndPos = i + 1;
	float colliderWidth = m_GridCellSize * (colliderEndPos - colliderBeginPos);
	float colliderHeight = m_GridCellSize * 4;
	glm::vec2 pos = { m_GridCellSize * colliderBeginPos, m_GridCellSize * m_LineNumber - m_GridCellSize * 0.5f };

	b2Filter filter{};
	filter.categoryBits = collisionGroup;

	m_GameObjectRef->AddComponent(new biggin::BoxCollider2d(
		m_GameObjectRef, { colliderWidth, colliderHeight }, false, b2_staticBody,
		{}, tag, pos, false, filter));
}

void MapLoader::ProcessLineItemsFile(const std::string& line) const
{
	for (int i{ 0 }; i < static_cast<int>(line.length()); ++i)
	{
		switch (static_cast<MapValues>(line[i]))
		{
		case MapValues::BurgerTop:
			SpawnBurgerPart(i, BurgerIngredients::BurgerTop);
			break;
		case MapValues::Lettuce:
			SpawnBurgerPart(i, BurgerIngredients::Lettuce);
			break;
		case MapValues::Meat:
			SpawnBurgerPart(i, BurgerIngredients::Meat);
			break;
		case MapValues::BurgerBottom:
			SpawnBurgerPart(i, BurgerIngredients::BurgerBottom);
			break;
		case MapValues::Pepper:
			break;
		default:;
		}
	}
}

void MapLoader::SpawnBurgerPart(int& i, BurgerIngredients ingredent) const
{
	const glm::vec2 pos = glm::vec2{ m_GridCellSize * i, m_GridCellSize * m_LineNumber - m_GridCellSize * 0.5f } + m_GameObjectRef->GetLocalPosition();
	//spawn burger top
	GameLoader::BurgerPrefab(ingredent, pos);
	//move to last letter of the burger
	i += Burger::GetBurgerSize() - 1;
}


