#include "EnemySpawner.h"
#include <iostream>
#pragma warning(push, 0)
#include <Box2D/Dynamics/b2Body.h>
#pragma warning(pop)
#include "BoxCollider2d.h"
#include "BurgerTimeCommands.hpp"
#include "EnemyColliderHandeler.h"
#include "EnemyMovement.h"
#include "GameObject.h"
#include "GameTime.h"
#include "MapLoader.h"
#include "PeterPepper.h"
#include "PossessGameObjectComponent.h"
#include "RemoveOnEvent.h"
#include "RenderComponent.h"
#include "Renderer.h"
#include "SpriteRenderComponent.h"
#include <vector>

#include "BurgerTimeMenuState.h"
#include <SDL_keycode.h>
#include "BurgerTimeCommands.hpp"
#include "imgui.h"
#include "GameLoader.h"
#include <thread>
#include "Biggin.h"
#include "BoxCollider2d.h"
#include "Burger.h"
#include "EnemySpawner.h"
#include "FpsCounter.h"
#include "SceneManager.h"
#include "Renderer.h"
#include "ResourceManager.h"
#include "TextComponent.h"
#include "GameObject.h"
#include "GameTime.h"
#include "Logger.h"
#include "Scene.h"
#include "HealthComponent.h"
#include "HealthUI.h"
#include "MapLoader.h"
#include "PeterPepper.h"
#include "RenderComponent.h"
#include "ScoreComponent.h"
#include "ScoreUI.h"
#include "SoundServiceLocator.h"
#include "SpriteRenderComponent.h"
#include "StatsAndAchievements.h"
#include "Subject.h"
#include "InputManager.h"


using namespace enemy;

enemy::EnemySpawner::EnemySpawner(biggin::GameObject* go, bool hasPossessedEnemy)
	:Component(go)
	, m_GameObjectRef(go)
	, m_HasPossessedHotDog(hasPossessedEnemy)
	, m_SpawnPossessedHotDog(hasPossessedEnemy)
	, m_DelayedSpawn(3.f, {})
	, m_DelayedSpawn2(3.f, {})
	, m_GameTimeRef{ biggin::GameTime::GetInstance() }
{
	m_DelayedSpawn.finished = true;
	m_DelayedSpawn2.finished = true;
}

//EnemySpawner::~EnemySpawner()
//{
//	//for preventing Lapsed listener problem
//	//a way better way to prevent this problem in our case would be to implement an event queue for the collisions but i won't have enough time for it now
//	//for (const auto& enemy : m_Enemies)
//	//	enemy->RemoveObservers({ this });
//}

void enemy::EnemySpawner::Initialize(biggin::GameObject*)
{
}

void enemy::EnemySpawner::Update()
{
	const auto deltaT = m_GameTimeRef.GetDeltaT();
	m_DelayedSpawn.Update(deltaT);
	m_DelayedSpawn2.Update(deltaT);

	PickEnemyAndLocation();
}

void enemy::EnemySpawner::OnNotify(Component* entity, const std::string& event)
{
	if (event == "EnemyDied")
	{
		const auto enemy = static_cast<EnemyColliderHandeler*>(entity);

		if (enemy->GetIsPossessed())
		{
			m_SpawnPossessedHotDog = true;
			return;
		}
		//std::erase(m_Enemies, enemy);
		//enemy->RemoveObservers({this});

		switch (enemy->GetEnemyType())
		{
		case EnemyType::HotDog:
			--m_AmntHotDogs;
			break;
		case EnemyType::Pickle:
			--m_AmntPickle;
			break;
		case EnemyType::Egg:
			--m_AmntEggs;
			break;
		}
	}
}

void enemy::EnemySpawner::RenderDebug()
{
	for (const auto& pos : m_SpawnPositions)
	{
		biggin::Renderer::GetInstance().RenderCircle(pos, 2);
	}
}

void enemy::EnemySpawner::ResetEnemyData()
{
	m_AmntEggs = 0;
	m_AmntHotDogs = 0;
	m_AmntPickle = 0;

	m_DelayedSpawn.finished = true;
	m_DelayedSpawn2.finished = true;
}

void enemy::EnemySpawner::FullReset()
{
	ResetEnemyData();

	m_SpawnPositions.clear();
	m_FreeSpawnPositions.clear();
}

void enemy::EnemySpawner::SpawnEnemyAtRandLocDelayed(EnemyType type)
{
	switch (type)
	{
	case EnemyType::HotDog:
		++m_AmntHotDogs;
		break;
	case EnemyType::Pickle:
		++m_AmntPickle;
		break;
	case EnemyType::Egg:
		++m_AmntEggs;
		break;
	}

	const int randomPosIdx = rand() % m_FreeSpawnPositions.size();
	const glm::vec2 chosenPos = m_FreeSpawnPositions[randomPosIdx];
	m_FreeSpawnPositions.erase(std::next(m_FreeSpawnPositions.begin(), randomPosIdx));
	if (m_DelayedSpawn.finished)
	{
		m_DelayedSpawn.Reset();
		m_DelayedSpawn.func = [this, chosenPos, type] {SpawnEnemy(type, chosenPos); };
	}
	else
	{
		m_DelayedSpawn2.Reset();
		m_DelayedSpawn2.func = [this, chosenPos, type] {SpawnEnemy(type, chosenPos); };
	}
}

void enemy::EnemySpawner::SpawnPossessedEnemyAtRandLocDelayed()
{
	m_SpawnPossessedHotDog = false;

	const int randomPosIdx = rand() % m_FreeSpawnPositions.size();
	const glm::vec2 chosenPos = m_FreeSpawnPositions[randomPosIdx];
	m_FreeSpawnPositions.erase(std::next(m_FreeSpawnPositions.begin(), randomPosIdx));
	if (m_DelayedSpawn.finished)
	{
		m_DelayedSpawn.Reset();
		m_DelayedSpawn.func = [this, chosenPos] {SpawnPossessedEnemy(chosenPos); };
	}
	else
	{
		m_DelayedSpawn2.Reset();
		m_DelayedSpawn2.func = [this, chosenPos] {SpawnPossessedEnemy(chosenPos); };
	}
}

void enemy::EnemySpawner::PickEnemyAndLocation()
{
	if (!m_DelayedSpawn.finished && !m_DelayedSpawn2.finished || m_FreeSpawnPositions.empty())
		return;

	if (m_HasPossessedHotDog && m_SpawnPossessedHotDog)
	{
		SpawnPossessedEnemyAtRandLocDelayed();
	}
	else if (m_AmntEggs < m_Settings.maxEggs)
	{
		SpawnEnemyAtRandLocDelayed(EnemyType::Egg);
	}
	else if (m_AmntHotDogs < m_Settings.maxHotDogs)
	{
		SpawnEnemyAtRandLocDelayed(EnemyType::HotDog);
	}
	else if (m_AmntPickle < m_Settings.maxPickles)
	{
		SpawnEnemyAtRandLocDelayed(EnemyType::Pickle);
	}
}

void enemy::EnemySpawner::SpawnEnemy(EnemyType type, glm::vec2 pos)
{
	auto enemy = std::make_shared<biggin::GameObject>();
	enemy->Setname("Enemy");
	enemy->SetLocalPosition(pos);
	m_FreeSpawnPositions.push_back(pos);

	int columns{ 9 };
	enemy->AddComponent(new biggin::RenderComponent(enemy.get(), "BurgerTimeSpriteSheet.png"));
	auto enemySprite = new biggin::SpriteRenderComponent(enemy.get(), { columns,{0,0},{32,32} });
	enemySprite->AddAnimation(static_cast<int>(enemy::AnimationState::runHorizontal), { 2, 20 + (columns * 2) * static_cast<int>(type) });
	enemySprite->AddAnimation(static_cast<int>(enemy::AnimationState::runVertical), { 2, 18 + (columns * 2) * static_cast<int>(type) });
	enemySprite->AddAnimation(static_cast<int>(enemy::AnimationState::peppered), { 2, 31 + (columns * 2) * static_cast<int>(type) });
	enemySprite->AddAnimation(static_cast<int>(enemy::AnimationState::die), { 4, 27 + (columns * 2) * static_cast<int>(type) });
	enemySprite->SetCurrentSprite(0);
	enemy->AddComponent(enemySprite);

	enemy->AddComponent(new biggin::Subject(enemy.get()));
	b2Filter filter{};
	filter.maskBits = burgerTime::MapLoader::mapGroup; //ignore everything except for mapCollisionsGroup

	auto enemyMovComp = new EnemyMovement(enemy.get(), m_Settings.velocity);
	enemy->AddComponent(enemyMovComp);
	enemy->AddComponent(new biggin::BoxCollider2d(enemy.get(), { 27, 2 }, true, b2_dynamicBody, { enemyMovComp }
	, "ColliderTop", { 0, -17 }, true, filter));
	enemy->AddComponent(new biggin::BoxCollider2d(enemy.get(), { 27, 2 }, true, b2_dynamicBody, { enemyMovComp }
	, "ColliderBottom", { 0, 17 }, true, filter));
	enemy->AddComponent(new biggin::BoxCollider2d(enemy.get(), { 2, 25 }, true, b2_dynamicBody, { enemyMovComp }
	, "ColliderLeft", { -33, 0 }, true, filter));
	enemy->AddComponent(new biggin::BoxCollider2d(enemy.get(), { 2, 25 }, true, b2_dynamicBody, { enemyMovComp }
	, "ColliderRight", { 33, 0 }, true, filter));

	auto enemyCollisionHandeler = new EnemyColliderHandeler(enemy.get(), type, { this });
	enemy->AddComponent(enemyCollisionHandeler);
	b2Filter filterEnemyCollider{};
	filterEnemyCollider.categoryBits = burgerTime::MapLoader::mapCollisionGroup::mapIgnoreGroup;
	filterEnemyCollider.maskBits = Burger::BurgerCollisionGroup::burgerCollisionGroup | static_cast<int>(character::PeterPepper::PlayerCollisionGroup::playerCollisionGroup);
	enemy->AddComponent(new biggin::BoxCollider2d(enemy.get(), { 32, 16 }, true, b2_dynamicBody, { enemyCollisionHandeler }
	, "Enemy", { 0, 9 }, true, filterEnemyCollider));

	enemy->AddComponent(new biggin::RemoveOnEvent(enemy.get(), "FinishedLevel", "Map"));
	enemy->AddComponent(new biggin::RemoveOnEvent(enemy.get(), "EnemyDied", enemy, 2.f));

	m_GameObjectRef->GetSceneRef()->AddPending(enemy);

	//m_Enemies.emplace_back(enemyCollisionHandeler);
}

void enemy::EnemySpawner::SpawnPossessedEnemy(glm::vec2 pos)
{
	auto enemy = std::make_shared<biggin::GameObject>();
	enemy->Setname("Enemy");
	enemy->SetLocalPosition(pos);
	m_FreeSpawnPositions.push_back(pos);

	int columns{ 9 };
	enemy->AddComponent(new biggin::RenderComponent(enemy.get(), "BurgerTimeSpriteSheet.png"));
	auto enemySprite = new biggin::SpriteRenderComponent(enemy.get(), { columns,{0,0},{32,32} });
	enemySprite->AddAnimation(static_cast<int>(enemy::AnimationState::runHorizontal), { 2, 20 + (columns * 2) * static_cast<int>(EnemyType::HotDog) });
	enemySprite->AddAnimation(static_cast<int>(enemy::AnimationState::runVertical), { 2, 18 + (columns * 2) * static_cast<int>(EnemyType::HotDog) });
	enemySprite->AddAnimation(static_cast<int>(enemy::AnimationState::peppered), { 2, 31 + (columns * 2) * static_cast<int>(EnemyType::HotDog) });
	enemySprite->AddAnimation(static_cast<int>(enemy::AnimationState::die), { 4, 27 + (columns * 2) * static_cast<int>(EnemyType::HotDog) });
	enemySprite->SetCurrentSprite(0);
	enemy->AddComponent(enemySprite);

	enemy->AddComponent(new biggin::Subject(enemy.get()));
	b2Filter filter{};
	filter.maskBits = burgerTime::MapLoader::mapGroup; //ignore everything except for mapCollisionsGroup

	auto movementCompoent = new biggin::PossessGameObjectComponent(enemy.get(), 100);
	enemy->AddComponent(movementCompoent);

	auto enemyCollisionHandeler = new EnemyColliderHandeler(enemy.get(), EnemyType::HotDog, { this });
	enemy->AddComponent(enemyCollisionHandeler);
	b2Filter filterEnemyCollider{};
	filterEnemyCollider.categoryBits = burgerTime::MapLoader::mapCollisionGroup::mapIgnoreGroup;
	filterEnemyCollider.maskBits = Burger::BurgerCollisionGroup::burgerCollisionGroup | static_cast<int>(character::PeterPepper::PlayerCollisionGroup::playerCollisionGroup);
	enemy->AddComponent(new biggin::BoxCollider2d(enemy.get(), { 32, 16 }, true, b2_dynamicBody, { enemyCollisionHandeler }
	, "Enemy", { 0, 9 }, true, filterEnemyCollider));

	enemy->AddComponent(new biggin::RemoveOnEvent(enemy.get(), "FinishedLevel", "Map"));
	enemy->AddComponent(new biggin::RemoveOnEvent(enemy.get(), "EnemyDied", enemy, 2.f));

	m_GameObjectRef->GetSceneRef()->AddPending(enemy);

	//only spawns once can keep moving

	//Movement bindings
	biggin::InputManager::GetInstance().MapActionKey({biggin::ActionState::Hold, biggin::ControllerButton::None, 1, SDLK_UP },
	                                                 std::make_unique<MoveCommand>(movementCompoent, MoveCommand::ActionDirection::Up));
	biggin::InputManager::GetInstance().MapActionKey({biggin::ActionState::Hold, biggin::ControllerButton::None, 1, SDLK_LEFT },
	                                                 std::make_unique<MoveCommand>(movementCompoent, MoveCommand::ActionDirection::Left));
	biggin::InputManager::GetInstance().MapActionKey({biggin::ActionState::Hold, biggin::ControllerButton::None, 1, SDLK_DOWN },
	                                                 std::make_unique<MoveCommand>(movementCompoent, MoveCommand::ActionDirection::Down));
	biggin::InputManager::GetInstance().MapActionKey({biggin::ActionState::Hold, biggin::ControllerButton::None, 1, SDLK_RIGHT },
	                                                 std::make_unique<MoveCommand>(movementCompoent, MoveCommand::ActionDirection::Right));

	biggin::InputManager::GetInstance().MapActionKey({biggin::ActionState::ThumbL, biggin::ControllerButton::None, 1 },
	                                                 std::make_unique<MoveCommand>(movementCompoent, MoveCommand::ActionDirection::All));

	//m_Enemies.emplace_back(enemyCollisionHandeler);
}