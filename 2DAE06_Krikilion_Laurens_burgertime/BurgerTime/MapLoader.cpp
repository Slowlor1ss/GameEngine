#include "MapLoader.h"
#pragma warning(push, 0)
#include <fstream>
#include <Box2D/Dynamics/b2Body.h>
#include <document.h>
#include <istreamwrapper.h>
#pragma warning(pop)
#include "Biggin.h"
#include "BoxCollider2d.h"
#include "Burger.h"
#include "GameLoader.h"
#include "GameObject.h"
#include "Logger.h"
#include "PeterPepper.h"
#include "Observer.h"
#include "EnemySpawner.h"
#include "GameTime.h"
#include "PepperShooter.h"
#include "RemoveOnEvent.h"
#include "RenderComponent.h"
#include "SoundServiceLocator.h"

using namespace burgerTime;

//Map loader works by reading a text file generated from a image with a small badly coded python script :D
MapLoader::MapLoader(biggin::GameObject* go, const std::vector<Observer*>& observers)
	: Component(go)
	, m_CurrentLevelIdx(0)
	, m_GameObjectRef(go)
	, m_GameTimeRef{biggin::GameTime::GetInstance() }
	, m_pNotifier(go->GetComponent<biggin::Subject>())
{
	if (m_pNotifier == nullptr)
		Logger::GetInstance().LogErrorAndBreak("Missing Subject Component");

	for (const auto observer : observers)
		m_pNotifier->AddObserver(observer);

	ReadJsonLevelFile("../Data/Levels.json");
}

burgerTime::MapLoader::~MapLoader()
{
	for (const auto& playerObject : m_PlayerRef)
	{
		playerObject->RemoveObserver(this);
	}
}

void MapLoader::Initialize(biggin::GameObject*)
{
}

void MapLoader::Start()
{
	const auto spawnerGo = m_GameObjectRef->GetSceneRef()->FindGameObjectWithName("EnemySpawner");
	if (spawnerGo != nullptr)
	{
		m_EnemySpawnerRef = spawnerGo->GetComponent<enemy::EnemySpawner>();
		if (m_CurrentLevelIdx < static_cast<int>(m_EnemySettingsPerLevel.size()))
			m_EnemySpawnerRef->SetMaxEnemies(m_EnemySettingsPerLevel[m_CurrentLevelIdx]);
	}

	if (m_LevelPaths.empty())
		Logger::GetInstance().LogErrorAndBreak("No level paths were found; add levels in the Levels.json file");

	m_BackgroundImgRef = m_GameObjectRef->GetComponent<biggin::RenderComponent>();
	m_BackgroundImgRef->SetOffset(-m_GameObjectRef->GetWorldPosition());

	m_PlayerRef = m_GameObjectRef->GetSceneRef()->FindGameObjectsWithName("Player");
	if (m_PlayerRef.empty())
		Logger::GetInstance().LogWarning("No player found");
	else
	{
		//every player needs a respawner in case they would die at the same time
		for (size_t i{0}; i < m_PlayerRef.size(); ++i)
		{
			m_PlayerRef[i]->AddObserver(this);
			m_DelayedRespawns.emplace_back(utils::DelayedCallback(2.f, {}));
			m_DelayedRespawns.back().finished = true;
		}
	}

	LoadMap(m_LevelPaths[m_CurrentLevelIdx]);
}

void MapLoader::OnNotify(Component* entity, const std::string& event)
{
	if (event == "BurgerReachedEnd")
	{
		++m_AmntLandedParts;
		if (m_AmntLandedParts == m_AmntBurgerParts)
		{
			//player finished level
			m_LoadNextLevel = true;
			for (auto& playerGo : m_PlayerRef)
			{
				auto player = playerGo->GetComponent<character::PeterPepper>();
				if (player && player->IsDead())
				{
					player->IncreaseHealth();
					player->RespawnPlayer(m_PlayerSpawnLocation);
				}

				auto pepperShooter = playerGo->GetComponent<PepperShooter>();
				if (pepperShooter != nullptr)
					pepperShooter->IncreasePepper();
			}
			SoundServiceLocator::GetSoundSystem().StopAll();
			SoundServiceLocator::GetSoundSystem().Play("win.wav", 0.2f);
		}
	}
	else if (event  == "PlayerDied")
	{
		if (m_PlayerRef.size() == 1)
			SoundServiceLocator::GetSoundSystem().StopMusic();

		for (auto& delayedCallback : m_DelayedRespawns)
		{
			//if callback is already re-spawning another player -> use next callback
			if (!delayedCallback.finished)
				continue;

			delayedCallback.func = [this, entity]{
				auto player = static_cast<character::PeterPepper*>(entity);
				if (player->GetHealth() > 0)
				{
					player->RespawnPlayer(m_PlayerSpawnLocation);
				}
				else
				{
					//Check if all players are dead
					bool gameOver = true;
					for (auto& playerGo : m_PlayerRef)
					{
						auto playerComponent = playerGo->GetComponent<character::PeterPepper>();
						if (playerComponent && playerComponent->GetHealth() > 0)
							gameOver = false;
					}

					if (gameOver)
					{
						for (auto& playerGo : m_PlayerRef)
							if (const auto scoreComponent = playerGo->GetComponent<ScoreComponent>())
								scoreComponent->SaveHighScore();

						m_pNotifier->notify(this, "GameOver");
					}
				}

				auto* scene = m_GameObjectRef->GetSceneRef();
				for (auto& enemy : scene->FindGameObjectsWithName("Enemy"))
				{
					scene->Remove(enemy.get());
				}

				m_EnemySpawnerRef->ResetEnemyData();

				//we only restart the music in single player as we dont stop the music in upon death in multiplayer
				if (m_PlayerRef.size() == 1)
					SoundServiceLocator::GetSoundSystem().Play("main.wav", 0.2f, false);
			};
			delayedCallback.Reset();
			break;
		}
	}
}

void MapLoader::Update()
{
	const auto deltaT = m_GameTimeRef.GetDeltaT();
	for (auto& delayedCallback : m_DelayedRespawns)
		delayedCallback.Update(deltaT);

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
	m_EnemySpawnerRef->FullReset();
}

void MapLoader::LoadNext()
{
	++m_CurrentLevelIdx;
	if (m_CurrentLevelIdx < static_cast<int>(m_LevelPaths.size()))
	{
		if (m_CurrentLevelIdx < static_cast<int>(m_EnemySettingsPerLevel.size()))
			m_EnemySpawnerRef->SetMaxEnemies(m_EnemySettingsPerLevel[m_CurrentLevelIdx]);
		LoadMap(m_LevelPaths[m_CurrentLevelIdx]);
	}
	else
	{
		//end of game
		m_pNotifier->notify(this, "GameOver");
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
		//if it has no level path there's no point in continuing for that block
		if (!(*jsonDocItr).HasMember("levelPath"))
			continue;

		m_LevelPaths.emplace_back((*jsonDocItr)["levelPath"].GetString());

		if ((*jsonDocItr).HasMember("backgroundImage"))
			m_ImagePaths.emplace_back((*jsonDocItr)["backgroundImage"].GetString());

		enemy::EnemySpawner::EnemySettings settings{};
		if ((*jsonDocItr).HasMember("maxHotDogs"))
			settings.maxHotDogs = (*jsonDocItr)["maxHotDogs"].GetInt();

		if ((*jsonDocItr).HasMember("maxEggs"))
			settings.maxEggs = (*jsonDocItr)["maxEggs"].GetInt();

		if ((*jsonDocItr).HasMember("maxPickles"))
			settings.maxPickles = (*jsonDocItr)["maxPickles"].GetInt();

		if ((*jsonDocItr).HasMember("speed"))
			settings.velocity = (*jsonDocItr)["speed"].GetFloat();

		m_EnemySettingsPerLevel.emplace_back(settings);

	}

	//if (m_LevelPaths.size() != m_ImagePaths.size())
	//	Logger::GetInstance().LogErrorAndBreak("Not every level has a background image");
}

void MapLoader::LoadMap(const std::string& file)
{
	m_BackgroundImgRef->SetTexture(m_ImagePaths[m_CurrentLevelIdx]);

	//remove the extension if there is one
	const std::string rawName = file.substr(0, file.find_last_of('.'));

    ReadLevelFile(rawName + ".txt");
    ReadItemsFile(rawName + "items.txt");

	CreateMapSideColliders();

	SoundServiceLocator::GetSoundSystem().Play("main.wav", 0.2f, false);
}

void MapLoader::CreateMapSideColliders()
{
	b2Filter filter{};
	filter.categoryBits = mapCollisionGroup::platformGroup;
	filter.maskBits = 0xFFFF ^ mapCollisionGroup::mapIgnoreGroup; //Ignore group 6

	//left
	m_GameObjectRef->AddComponentPending(new biggin::BoxCollider2d(
		m_GameObjectRef, { m_GridCellSize, 1000 }, false, b2_staticBody, {}, {},
		{ -m_GridCellSize, 0 }, false, filter));
	//right
	m_GameObjectRef->AddComponentPending(new biggin::BoxCollider2d(
		m_GameObjectRef, { m_GridCellSize, 1000 }, false, b2_staticBody, {}, {},
		{ m_MapMaxWidthIdx * m_GridCellSize, 0 }, false, filter));
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

void MapLoader::ProcessLineMapFile(const std::string& line)
{
	const int lineWidth{ static_cast<int>(line.length()) };
    int colliderBeginPos{ 0 };
    
	for (int i{0}; i < lineWidth; ++i)
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
			MakeCollider(i, colliderBeginPos, 2, mapCollisionGroup::platformGroup, "Catcher");
			break;
		case MapValues::EnemySpawner:
			SpawnEnemySpawner(i, lineWidth-1);
			break;
		case MapValues::Player:
			m_PlayerSpawnLocation = glm::vec2{ m_GridCellSize * i + m_GridCellSize * 0.5f, m_GridCellSize * m_LineNumber - m_GridCellSize * 0.5f } + m_GameObjectRef->GetLocalPosition();
            for (auto& playerGo : m_PlayerRef)
	            playerGo->SetLocalPosition(m_PlayerSpawnLocation);
            break;
        default: ;
		}
	}

	if (m_MapMaxWidthIdx < (lineWidth - 1))
		m_MapMaxWidthIdx = lineWidth;
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

void MapLoader::MakeCollider(int idx, int colliderBeginPos, int sizeMultiplier, unsigned short collisionGroup, std::string tag) const
{
	const int colliderEndPos = idx + 1;
	float colliderWidth = m_GridCellSize * (colliderEndPos - colliderBeginPos);
	float colliderHeight = m_GridCellSize * sizeMultiplier;//4;
	const glm::vec2 pos = { m_GridCellSize * colliderBeginPos, (m_GridCellSize * m_LineNumber) - (m_GridCellSize * 0.5f )};

	b2Filter filter{};
	filter.categoryBits = collisionGroup;
	filter.maskBits = 0xFFFF ^ mapCollisionGroup::mapIgnoreGroup; //Ignore group 6

	m_GameObjectRef->AddComponentPending(new biggin::BoxCollider2d(
		m_GameObjectRef, { colliderWidth, colliderHeight }, false, b2_staticBody,
		{}, tag, pos, false, filter));
}

void MapLoader::SpawnBurgerPart(int& idx, BurgerIngredients ingredent)
{
	++m_AmntBurgerParts;

	const glm::vec2 pos = glm::vec2{ m_GridCellSize * idx, (m_GridCellSize * m_LineNumber) - (m_GridCellSize * 0.5f )} + m_GameObjectRef->GetLocalPosition();
	//spawn burger top
	burgerTime::BurgerPrefab(ingredent, pos, { this });
	//move to last letter of the burger
	idx += Burger::GetBurgerSize() - 1;
}

void MapLoader::SpawnEnemySpawner(int idx, int mapMaxWidthIdx) const
{
	if (m_EnemySpawnerRef == nullptr)
		return;

	float offset = idx < (mapMaxWidthIdx / 2) ? -m_GridCellSize*2 : m_GridCellSize*3;
	const glm::vec2 pos = glm::vec2{ m_GridCellSize * idx + offset, m_GridCellSize * m_LineNumber - m_GridCellSize * 0.5f } + m_GameObjectRef->GetLocalPosition();
	m_EnemySpawnerRef->AddSpawnLocation(pos);
}

void burgerTime::BurgerPrefab(BurgerIngredients ingredient, glm::vec2 pos, const std::vector<biggin::Observer*>& observers)
{
	auto& scene = biggin::SceneManager::GetInstance().GetActiveScene();


	//making the burger
	const auto burger = std::make_shared<biggin::GameObject>();
	burger->SetLocalPosition(pos);
	burger->AddComponent(new biggin::Subject(burger.get())); //used to notify when a burger has reached the catcher
	const auto burgerComponent = new Burger(burger.get(), ingredient, observers);
	burger->AddComponent(burgerComponent);
	burger->AddComponent(new biggin::RemoveOnEvent(burger.get(), "FinishedLevel", "Map"));
	scene.AddPending(burger);

	//adding the 4 child components out burger par uses
	b2Filter filter{};
	filter.maskBits = 0xFFFF ^ Burger::burgerIgnoreGroup; //Ignore group 1
	filter.categoryBits = Burger::burgerCollisionGroup; //set self to group 2

	for (int i{ 0 }; i < Burger::GetBurgerSize(); ++i)
	{
		const auto burgerCell = std::make_shared<biggin::GameObject>(burger.get());
		burgerCell->AddComponent(new biggin::Subject(burgerCell.get()));
		burgerCell->AddComponent(new biggin::RenderComponent(burgerCell.get()));
		burgerCell->AddComponent(
			new biggin::BoxCollider2d(
				burgerCell.get(), { MapLoader::GetGridSize(), MapLoader::GetGridSize() - 5 }, true, b2_dynamicBody,
				{ burgerComponent }, "Burger", { 0, 5 }, false, filter)
		);
		burgerCell->AddComponent(new biggin::RemoveOnEvent(burgerCell.get(), "FinishedLevel", "Map"));
		scene.AddPending(burgerCell);
	}
}

//void burgerTime::EnemySpawnerPrefab(glm::vec2 pos)
//{
//	auto& scene = biggin::SceneManager::GetInstance().GetActiveScene();
//
//
//	//making the burger
//	const auto spawner = std::make_shared<biggin::GameObject>();
//	spawner->SetLocalPosition(pos);
//
//	spawner->AddComponent(new biggin::RemoveOnEvent(spawner.get(), "FinishedLevel", "Map"));
//	
//	scene.AddPending(spawner);
//}
