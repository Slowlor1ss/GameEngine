#include "BigginPCH.h"
#include "PeterPepper.h"
#include "GameObject.h"
#include "GameTime.h"
#include "HealthComponent.h"
#include "InputManager.h"
#include "Logger.h"
#include "SpriteRenderComponent.h"

character::PeterPepper::PeterPepper(biggin::GameObject* go, float movementSpeed) : Component(go)
	, m_IsDead(false)
	, m_Speed(movementSpeed)
	, m_PlayerIndex(m_AmntPlayers)
	, m_pHealthComp(nullptr)
	, m_pSpriteComp(nullptr)
{
	++m_AmntPlayers;
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
	//GetGameObject()->TranslateLocalPosition(m_Velocity*GameTime::GetFixedTimeStep());
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

void character::PeterPepper::UpdateMovementDirection()
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

	m_CurrMovementDir = MoveDirection::None;
}

void character::PeterPepper::FixedUpdate()
{
	if (m_IsDead)
	{
		m_CurrAnimState = AnimationState::Die;
		return;
	}

	UpdateMovementDirection();

	UpdateAnimationState();

	//TODO: make update animation function
	//get refernce to sprite renderer update that and hope it works goodnight
	const bool isIdle = m_CurrAnimState == AnimationState::Idle;
	m_pSpriteComp->SetCurrentSprite(static_cast<int>(m_CurrAnimState));

	m_pSpriteComp->SetPause(isIdle);
	if (isIdle)
		return;

	m_pSpriteComp->SetFlip(m_CurrMovementDir == MoveDirection::Right ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);

	GetGameObject()->TranslateLocalPosition(m_Velocity * GameTime::GetFixedTimeStep());

	m_Velocity = {0,0};
}

void character::PeterPepper::Damage()
{
	m_pHealthComp->Damage();
	m_IsDead = true; //TODO: implement re-spawning and reset is dead
}

void character::PeterPepper::OnNotify(const Component* /*entity*/, const std::string& event)
{
	if (event == "BeginContact")
	{
		std::cout << "ovelap started\n";
	}
	else if (event == "EndContact")
	{
		std::cout << "ovelap ended\n";
	}
}
