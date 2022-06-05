#include "EnemySpawner.h"
#include <iostream>
#pragma warning(push, 0)
#include <Box2D/Dynamics/b2Body.h>
#pragma warning(pop)
#include "BoxCollider2d.h"
#include "EnemyColliderHandeler.h"
#include "EnemyMovement.h"
#include "GameObject.h"
#include "GameTime.h"
#include "MapLoader.h"
#include "PeterPepper.h"
#include "RemoveOnEvent.h"
#include "RenderComponent.h"
#include "Renderer.h"
#include "SpriteRenderComponent.h"

EnemySpawner::EnemySpawner(biggin::GameObject* go, bool hasPossessedEnemy)
	:Component(go)
	,m_GameObjectRef(go)
	,m_GameTimeRef{biggin::GameTime::GetInstance() }
	,m_DelayedSpawn(3.f, {})
	,m_DelayedSpawn2(3.f, {})
	,m_HasPossessedHotDog(hasPossessedEnemy)
	,m_SpawnPossessedHotDog(hasPossessedEnemy)
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

void EnemySpawner::Initialize(biggin::GameObject*)
{
}

void EnemySpawner::Update()
{
	const auto deltaT = m_GameTimeRef.GetDeltaT();
	m_DelayedSpawn.Update(deltaT);
	m_DelayedSpawn2.Update(deltaT);

	PickEnemyAndLocation();
}

void EnemySpawner::OnNotify(Component* entity, const std::string& event)
{
	if (event == "EnemyDied")
	{
		const auto enemy = static_cast<EnemyColliderHandeler*>(entity);

		//std::erase(m_Enemies, enemy);
		//enemy->RemoveObservers({this});

		switch(enemy->GetEnemyType())
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

void EnemySpawner::RenderDebug()
{
	for (const auto& pos : m_SpawnPositions)
	{
		biggin::Renderer::GetInstance().RenderCircle(pos, 2);
	}
}

void EnemySpawner::ResetEnemyData()
{
	m_AmntEggs = 0;
	m_AmntHotDogs = 0;
	m_AmntPickle = 0;

	m_DelayedSpawn.finished = true;
	m_DelayedSpawn2.finished = true;
}

void EnemySpawner::FullReset()
{
	ResetEnemyData();

	m_SpawnPositions.clear();
	m_FreeSpawnPositions.clear();
}

void EnemySpawner::SpawnEnemyAtRandLocDelayed(EnemyType type)
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
		m_DelayedSpawn.func = [this, chosenPos, type]{SpawnEnemy(type, chosenPos);};
	}
	else
	{
		m_DelayedSpawn2.Reset();
		m_DelayedSpawn2.func = [this, chosenPos, type]{SpawnEnemy(type, chosenPos); };
	}
}

void EnemySpawner::SpawnPossessedEnemyAtRandLocDelayed()
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

void EnemySpawner::PickEnemyAndLocation()
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

void EnemySpawner::SpawnEnemy(EnemyType type, glm::vec2 pos)
{
	auto enemy = std::make_shared<biggin::GameObject>();
	enemy->Setname("Enemy");
	enemy->SetLocalPosition(pos);
	m_FreeSpawnPositions.push_back(pos);

	int columns{ 9 };
	enemy->AddComponent(new biggin::RenderComponent(enemy.get(), "BurgerTimeSpriteSheet.png"));
	auto enemySprite = new biggin::SpriteRenderComponent(enemy.get(), { columns,{0,0},{32,32} });
	enemySprite->AddAnimation(static_cast<int>(EnemyMovement::AnimationState::runHorizontal), { 2, 20 + (columns * 2)*static_cast<int>(type)});
	enemySprite->AddAnimation(static_cast<int>(EnemyMovement::AnimationState::runVertical), { 2, 18 + (columns * 2) * static_cast<int>(type) });
	enemySprite->AddAnimation(static_cast<int>(EnemyMovement::AnimationState::peppered), { 2, 31 + (columns * 2) * static_cast<int>(type) });
	enemySprite->AddAnimation(static_cast<int>(EnemyMovement::AnimationState::die), { 4, 27 + (columns * 2) * static_cast<int>(type) });
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

	auto enemyCollisionHandeler = new EnemyColliderHandeler(enemy.get(), type, {this});
	enemy->AddComponent(enemyCollisionHandeler);
	b2Filter filterEnemyCollider{};
	filterEnemyCollider.categoryBits = burgerTime::MapLoader::mapCollisionGroup::mapIgnoreGroup;
	filterEnemyCollider.maskBits = Burger::BurgerCollisionGroup::burgerCollisionGroup | static_cast<int>(character::PeterPepper::PlayerCollisionGroup::playerCollisionGroup);
	enemy->AddComponent(new biggin::BoxCollider2d(enemy.get(), { 32, 16 }, true, b2_dynamicBody, { enemyCollisionHandeler }
	                                              , "Enemy", {0, 9 }, true, filterEnemyCollider));

	enemy->AddComponent(new biggin::RemoveOnEvent(enemy.get(), "FinishedLevel", "Map"));
	enemy->AddComponent(new biggin::RemoveOnEvent(enemy.get(), "EnemyDied", enemy, 2.f));

	m_GameObjectRef->GetSceneRef()->AddPending(enemy);

	//m_Enemies.emplace_back(enemyCollisionHandeler);
}

void EnemySpawner::SpawnPossessedEnemy(glm::vec2 pos)
{
	auto enemy = std::make_shared<biggin::GameObject>();
	enemy->Setname("Enemy");
	enemy->SetLocalPosition(pos);
	m_FreeSpawnPositions.push_back(pos);

	int columns{ 9 };
	enemy->AddComponent(new biggin::RenderComponent(enemy.get(), "BurgerTimeSpriteSheet.png"));
	auto enemySprite = new biggin::SpriteRenderComponent(enemy.get(), { columns,{0,0},{32,32} });
	enemySprite->AddAnimation(static_cast<int>(EnemyMovement::AnimationState::runHorizontal), { 2, 20 + (columns * 2) * static_cast<int>(EnemyType::HotDog) });
	enemySprite->AddAnimation(static_cast<int>(EnemyMovement::AnimationState::runVertical), { 2, 18 + (columns * 2) * static_cast<int>(EnemyType::HotDog) });
	enemySprite->AddAnimation(static_cast<int>(EnemyMovement::AnimationState::peppered), { 2, 31 + (columns * 2) * static_cast<int>(EnemyType::HotDog) });
	enemySprite->AddAnimation(static_cast<int>(EnemyMovement::AnimationState::die), { 4, 27 + (columns * 2) * static_cast<int>(EnemyType::HotDog) });
	enemySprite->SetCurrentSprite(0);
	enemy->AddComponent(enemySprite);

	enemy->AddComponent(new biggin::Subject(enemy.get()));
	b2Filter filter{};
	filter.maskBits = burgerTime::MapLoader::mapGroup; //ignore everything except for mapCollisionsGroup



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

	//m_Enemies.emplace_back(enemyCollisionHandeler);
}
