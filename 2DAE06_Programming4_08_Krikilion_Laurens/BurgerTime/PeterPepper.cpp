#include "BigginPCH.h"
#include "PeterPepper.h"
#include "BoxCollider2d.h"
#include "GameObject.h"
#include "GameTime.h"
#include "HealthComponent.h"
#include "InputManager.h"
#include "Logger.h"
#include "MapLoader.h"
#include "SpriteRenderComponent.h"

character::PeterPepper::PeterPepper(biggin::GameObject* go, float movementSpeed) : Component(go)
	, m_IsDead(false)
	, m_Speed(movementSpeed)
	, m_PlayerIndex(m_AmntPlayers)
	, m_pHealthComp(nullptr)
	, m_pSpriteComp(nullptr)
	, m_pNotifier(go->GetComponent<biggin::Subject>())
{
	if (m_pNotifier == nullptr)
		Logger::GetInstance().LogErrorAndBreak("Missing Subject Component");
	++m_AmntPlayers;
	m_pGameObjectRef = go;
}

void character::PeterPepper::Initialize(biggin::GameObject* go)
{
	m_pSpriteComp = go->GetComponent<biggin::SpriteRenderComponent>();
	if (m_pSpriteComp == nullptr)
		Logger::GetInstance().LogErrorAndBreak("Missing SpriteRenderComponent");

	m_pHealthComp = go->GetComponent<biggin::HealthComponent>();
	if (m_pHealthComp == nullptr)
		Logger::GetInstance().LogWarning("Missing HealthComponent");
}

void character::PeterPepper::Update()
{
	if (m_IsDead)
		return;

	//negate y pos because (0,0) is at the top left
	//const glm::vec2 velocity = biggin::InputManager::GetInstance().GetLThumb(m_PlayerIndex) * glm::vec2{1,-1};
	//m_pGameObjectRef->TranslateLocalPosition(m_Velocity*GameTime::GetFixedTimeStep());
	//m_Velocity = {0,0};
}

void character::PeterPepper::UpdateAnimationState()
{
	switch (m_CurrMovementDir)
	{
	case MoveDirection::Left:
	case MoveDirection::Right:
		m_CurrAnimState = AnimationState::RunHorizontal;
		break;
	case MoveDirection::Up: 
	case MoveDirection::Down:
		m_CurrAnimState = AnimationState::RunVertical;
		break;
	case MoveDirection::None:
		m_CurrAnimState = AnimationState::Idle;
		break;
	}
}

void character::PeterPepper::UpdateMovementDirectionState()
{
	if (m_Velocity.x != 0)
	{
		m_CurrMovementDir = m_Velocity.x > 0 ? MoveDirection::Right : MoveDirection::Left;
		return;
	}

	if (m_Velocity.y != 0)
	{
		m_CurrMovementDir = m_Velocity.y > 0 ? MoveDirection::Down : MoveDirection::Up;
		return;
	}

	m_LastMovementDir = m_CurrMovementDir;
	m_CurrMovementDir = MoveDirection::None;
}

void character::PeterPepper::FixedUpdate()
{
	if (m_IsDead)
		return;

	if (m_VerticalMovDisabled)
		m_Velocity *= glm::vec2{1, 0};
	else if(m_HorizontalMovDisabled)
		m_Velocity *= glm::vec2{ 0, 1 };

	UpdateMovementDirectionState();

	UpdateAnimationState();

	m_pSpriteComp->SetCurrentSprite(static_cast<int>(m_CurrAnimState));

	const bool isIdle = m_CurrAnimState == AnimationState::Idle;
	//m_pSpriteComp->SetPause(isIdle);
	if (isIdle)
		return;

	m_pSpriteComp->SetFlip(m_CurrMovementDir == MoveDirection::Right ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);

	m_pGameObjectRef->TranslateLocalPosition(m_Velocity * biggin::GameTime::GetFixedTimeStep());

	m_Velocity = {0,0};
}

void character::PeterPepper::SetPosition(const glm::vec2& pos) const
{
	m_pGameObjectRef->SetLocalPosition(pos);
}

void character::PeterPepper::RespawnPlayer(const glm::vec2& pos)
{
	m_IsDead = false;
	m_pSpriteComp->SetPause(false);
	SetPosition(pos);
}

void character::PeterPepper::Damage()
{
	if (!m_IsDead)
	{
		m_pHealthComp->Damage();
		m_IsDead = true; //TODO: implement re-spawning and reset is dead
		m_CurrAnimState = AnimationState::Die;
		m_pSpriteComp->SetCurrentSprite(static_cast<int>(m_CurrAnimState));
		m_pSpriteComp->SetFinishAndPause();
		m_pNotifier->notify(this, "PlayerDied");
	}
}

int character::PeterPepper::GetHealth() const
{
	return m_pHealthComp->GetLives();
}

void character::PeterPepper::ShootPepper()
{
	glm::vec2 pos{ m_pGameObjectRef->GetLocalPosition() };

	const auto dir = m_CurrMovementDir == MoveDirection::None ? m_LastMovementDir : m_CurrMovementDir;
	switch (dir)
	{
	case MoveDirection::Left:
		pos += glm::vec2{-burgerTime::MapLoader::GetGridSize(), 0};
		break;
	case MoveDirection::Right:
		pos += glm::vec2{ burgerTime::MapLoader::GetGridSize(), 0 };
		break;
	case MoveDirection::Up:
		pos += glm::vec2{ 0, -burgerTime::MapLoader::GetGridSize() };
		break;
	case MoveDirection::Down:
		pos += glm::vec2{ 0, burgerTime::MapLoader::GetGridSize()};
		break;
	}

	m_pPepperShooter->Shoot(pos);
}

void character::PeterPepper::OnNotify(Component* /*entity*/, const std::string& event)
{
	if (event == "BeginContact")
	{
		std::cout << "ovelap started PeterPepper\n";
		//if (static_cast<const biggin::BoxCollider2d*>(entity)->GetTag() == "DisableUp")
		//{
		//	m_VerticalMovDisabled = true;
		//}
	}
	else if (event == "EndContact")
	{
		std::cout << "ovelap ended PeterPepper\n";
		//TODO: properly remove this also the variables connected to it
		//if (static_cast<const biggin::BoxCollider2d*>(entity)->GetTag() == "DisableUp")
		//{
		//	m_VerticalMovDisabled = false;
		//}
	}
}
