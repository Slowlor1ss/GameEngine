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
#include "Observer.h"
#include <document.h>
#include <istreamwrapper.h>
#include "RenderComponent.h"

//Map loader works by reading a text file generated from a image with a small badly coded python script :D
MapLoader::MapLoader(biggin::GameObject* go, const std::vector<Observer*>& observers)
	: Component(go)
	, m_CurrentLevelIdx(0)
	, m_GameObjectRef(go)
	, m_pNotifier(go->GetComponent<biggin::Subject>())
{
	if (m_pNotifier == nullptr)
		Logger::GetInstance().LogErrorAndBreak("Missing Subject Component");

	for (const auto observer : observers)
		m_pNotifier->AddObserver(observer);

	ReadJsonLevelFile("../Data/Levels.json");
}

void MapLoader::Initialize(biggin::GameObject*)
{
}

void MapLoader::Start()
{
	if (m_LevelPaths.empty())
		Logger::GetInstance().LogErrorAndBreak("No level paths were found; add levels in the Levels.json file");

	m_BackgroundImgRef = m_GameObjectRef->GetComponent<biggin::RenderComponent>();

	m_BackgroundImgRef->SetOffset(-m_GameObjectRef->GetWorldPosition());
	m_PlayerRef = m_GameObjectRef->GetSceneRef()->FindGameObjectsWithName("Player");
	if (m_PlayerRef.empty())
		Logger::GetInstance().LogWarning("No player found");

	LoadMap(m_LevelPaths[m_CurrentLevelIdx]);
}

void MapLoader::OnNotify(Component* /*entity*/, const std::string& event)
{
	if (event == "BurgerReachedEnd")
	{
		if (m_AmntLandedParts == m_AmntBurgerParts)
		{
			//player finished level
			//m_pNotifier->notify(this, "FinishedLevel");
			m_LoadNextLevel = true;
			//ResetLevel();
		}
	}
}

void MapLoader::Update()
{
	if (m_LoadNextLevel)
	{
		m_pNotifier->notify(this, "FinishedLevel");
		ResetLevel();
		LoadNext();
		m_LoadNextLevel = false;
	}
}

void MapLoader::ResetLevel()
{

	m_GameObjectRef->RemoveComponentsPending<biggin::BoxCollider2d>();
	m_AmntBurgerParts = 0;
	m_AmntLandedParts = 0;
}

void MapLoader::LoadNext()
{
	++m_CurrentLevelIdx;
	if (m_CurrentLevelIdx < m_LevelPaths.size())
	{
		LoadMap(m_LevelPaths[m_CurrentLevelIdx]);
	}
	else
	{
		//end of game
	}
}

void MapLoader::ReadJsonLevelFile(const std::string& file)
{
	std::ifstream is{ file };

	if (!is)
		throw std::runtime_error("Couldn't open/find file with name " + file);

	// Handle file content
	rapidjson::Document jsonDoc;
	rapidjson::IStreamWrapper isw{ is };
	jsonDoc.ParseStream(isw);

	for (auto jsonDocItr = jsonDoc.Begin(); jsonDocItr != jsonDoc.End(); ++jsonDocItr)
	{
		m_LevelPaths.emplace_back((*jsonDocItr)["levelPath"].GetString());
		m_ImagePaths.emplace_back((*jsonDocItr)["backgroundImage"].GetString());
	}

	if (m_LevelPaths.size() != m_ImagePaths.size())
		Logger::GetInstance().LogErrorAndBreak("Not every level has a background image");
}

void MapLoader::LoadMap(const std::string& file)
{
	m_BackgroundImgRef->SetTexture(m_ImagePaths[m_CurrentLevelIdx]);

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
			MakeCollider( i, colliderBeginPos);
            break;
		case MapValues::ColliderBeginBig:
			colliderBeginPos = i;
			break;
		case MapValues::ColliderEndBig:
			MakeCollider(i, colliderBeginPos, 4);
			break;
		case MapValues::ExtraColliderStart:
			colliderBeginPos = i;
			break;
		case MapValues::ExtraColliderQuit:
			MakeCollider(i, colliderBeginPos, 4, Burger::burgerIgnoreGroup);
			break;
		case MapValues::CatcherBegin:
			colliderBeginPos = i;
			break;
		case MapValues::CatcherEnd:
			MakeCollider(i, colliderBeginPos, 1, mapCollisionGroup::platformGroup, "Catcher");
			break;
		case MapValues::Player:
			m_PlayerRef[0]->SetLocalPosition(glm::vec2{ m_GridCellSize * i, m_GridCellSize * m_LineNumber - m_GridCellSize * 0.5f } + m_GameObjectRef->GetLocalPosition());
			break;
        default: ;
		}
	}
}

void MapLoader::MakeCollider(int i, int colliderBeginPos, int sizeMultiplier, unsigned short collisionGroup, std::string tag) const
{
	const int colliderEndPos = i + 1;
	float colliderWidth = m_GridCellSize * (colliderEndPos - colliderBeginPos);
	float colliderHeight = m_GridCellSize * sizeMultiplier;//4;
	const glm::vec2 pos = { m_GridCellSize * colliderBeginPos, m_GridCellSize * m_LineNumber - m_GridCellSize * 0.5f };

	b2Filter filter{};
	filter.categoryBits = collisionGroup;

	m_GameObjectRef->AddComponentPending(new biggin::BoxCollider2d(
		m_GameObjectRef, { colliderWidth, colliderHeight }, false, b2_staticBody,
		{}, tag, pos, false, filter));
}

void MapLoader::ProcessLineItemsFile(const std::string& line)
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
		case MapValues::Cheese:
			SpawnBurgerPart(i, BurgerIngredients::Cheese);
			break;
		case MapValues::Tomato:
			SpawnBurgerPart(i, BurgerIngredients::Tomato);
			break;
		case MapValues::Pepper:
			break;
		default:;
		}
	}
}

void MapLoader::SpawnBurgerPart(int& i, BurgerIngredients ingredent)
{
	++m_AmntBurgerParts;

	const glm::vec2 pos = glm::vec2{ m_GridCellSize * i, m_GridCellSize * m_LineNumber - m_GridCellSize * 0.5f } + m_GameObjectRef->GetLocalPosition();
	//spawn burger top
	GameLoader::BurgerPrefab(ingredent, pos, { this });
	//move to last letter of the burger
	i += Burger::GetBurgerSize() - 1;
}


