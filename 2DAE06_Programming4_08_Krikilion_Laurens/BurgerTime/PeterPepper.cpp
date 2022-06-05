#include "BigginPCH.h"
#include "PeterPepper.h"
#include "BoxCollider2d.h"
#include "GameObject.h"
#include "GameTime.h"
#include "HealthComponent.h"
#include "InputManager.h"
#include "Logger.h"
#include "MapLoader.h"
#include "PepperShooter.h"
#include "PossessGameObjectComponent.h"
#include "SoundServiceLocator.h"
#include "SpriteRenderComponent.h"

character::PeterPepper::PeterPepper(biggin::GameObject* go, float movementSpeed)
	: Component(go)
	, m_IsDead(false)
	, m_IsShooting(false)
	, m_DelayedStopShooting(0.3f, [this] {m_IsShooting = false; if(m_pMovementComp) 	m_pMovementComp->SetDisable(false); }, 1, true)
	, m_GameTimeRef(biggin::GameTime::GetInstance())
	, m_Speed(movementSpeed)
	, m_PlayerIndex(m_AmntPlayers)
	, m_pGameObjectRef(go)
	, m_pHealthComp(nullptr)
	, m_pPepperShooter(nullptr)
	, m_pSpriteComp(nullptr)
	, m_pNotifier(go->GetComponent<biggin::Subject>())
	, m_pMovementComp(nullptr)
{
	if (m_pNotifier == nullptr)
		Logger::GetInstance().LogErrorAndBreak("Missing Subject Component");
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

	m_pPepperShooter = go->GetComponent<burgerTime::PepperShooter>();
	if (m_pPepperShooter == nullptr)
		Logger::GetInstance().LogWarning("Missing PepperShooter");

	m_pMovementComp = go->GetComponent<biggin::PossessGameObjectComponent>();
	if (m_pMovementComp == nullptr)
		Logger::GetInstance().LogWarning("Missing PossessGameObjectComponent");
	else
	{
		m_pMovementComp->MapAnimToDir(biggin::PossessGameObjectComponent::MoveDirection::None, static_cast<int>(AnimationState::Idle));
		m_pMovementComp->MapAnimToDir(biggin::PossessGameObjectComponent::MoveDirection::Up, static_cast<int>(AnimationState::RunVertical));
		m_pMovementComp->MapAnimToDir(biggin::PossessGameObjectComponent::MoveDirection::Down, static_cast<int>(AnimationState::RunVertical));
		m_pMovementComp->MapAnimToDir(biggin::PossessGameObjectComponent::MoveDirection::Left, static_cast<int>(AnimationState::RunHorizontal));
		m_pMovementComp->MapAnimToDir(biggin::PossessGameObjectComponent::MoveDirection::Right, static_cast<int>(AnimationState::RunHorizontal));
	}
}

void character::PeterPepper::Update()
{
	if (m_IsDead || !m_IsShooting)
		return;

	m_DelayedStopShooting.Update(m_GameTimeRef.GetDeltaT());

	//negate y pos because (0,0) is at the top left
	//const glm::vec2 velocity = biggin::InputManager::GetInstance().GetLThumb(m_PlayerIndex) * glm::vec2{1,-1};
	//m_pGameObjectRef->TranslateLocalPosition(m_Velocity*GameTime::GetFixedTimeStep());
	//m_Velocity = {0,0};
}

//void character::PeterPepper::UpdateAnimationState()
//{
//	switch (m_CurrMovementDir)
//	{
//	case MoveDirection::Left:
//	case MoveDirection::Right:
//		m_CurrAnimState = AnimationState::RunHorizontal;
//		break;
//	case MoveDirection::Up: 
//	case MoveDirection::Down:
//		m_CurrAnimState = AnimationState::RunVertical;
//		break;
//	case MoveDirection::None:
//		m_CurrAnimState = AnimationState::Idle;
//		break;
//	}
//}
//
//void character::PeterPepper::UpdateMovementDirectionState()
//{
//	if (m_Velocity.x != 0)
//	{
//		m_CurrMovementDir = m_Velocity.x > 0 ? MoveDirection::Right : MoveDirection::Left;
//		return;
//	}
//
//	if (m_Velocity.y != 0)
//	{
//		m_CurrMovementDir = m_Velocity.y > 0 ? MoveDirection::Down : MoveDirection::Up;
//		return;
//	}
//
//	if (m_CurrMovementDir != MoveDirection::None)
//		m_LastMovementDir = m_CurrMovementDir;
//
//	m_CurrMovementDir = MoveDirection::None;
//}

void character::PeterPepper::FixedUpdate()
{
	if (m_IsDead || m_IsShooting)
		return;

	//if (m_VerticalMovDisabled)
	//	m_Velocity *= glm::vec2{1, 0};
	//else if(m_HorizontalMovDisabled)
	//	m_Velocity *= glm::vec2{ 0, 1 };

	//UpdateMovementDirectionState();

	//UpdateAnimationState();

	//m_pSpriteComp->SetCurrentSprite(static_cast<int>(m_CurrAnimState));

	//const bool isIdle = m_CurrAnimState == AnimationState::Idle;
	////m_pSpriteComp->SetPause(isIdle);
	//if (isIdle)
	//	return;

	//m_pSpriteComp->SetFlip(m_CurrMovementDir == MoveDirection::Right ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);

	//m_pGameObjectRef->TranslateLocalPosition(m_Velocity * biggin::GameTime::GetFixedTimeStep());

	//m_Velocity = {0,0};
}

void character::PeterPepper::SetVelocity(const glm::vec2& velocity)
{
	m_Velocity = glm::vec2{std::clamp(velocity.x, -m_Speed, m_Speed), std::clamp(velocity.y, -m_Speed, m_Speed) };
}

void character::PeterPepper::RespawnPlayer(const glm::vec2& pos)
{
	m_IsDead = false;
	if (m_pMovementComp)
		m_pMovementComp->SetDisable(false);
	//m_LastMovementDir = MoveDirection::Down;
	//m_CurrAnimState = AnimationState::Idle;
	//m_pSpriteComp->SetCurrentSprite(static_cast<int>(m_CurrAnimState));
	m_pSpriteComp->SetPause(false);
	m_pGameObjectRef->SetLocalPosition(pos);
}

void character::PeterPepper::Damage()
{
	if (!m_IsDead)
	{
		SoundServiceLocator::GetSoundSystem().Play("die.wav", 0.2f);
		m_pHealthComp->Damage();
		m_IsDead = true;
		//m_CurrAnimState = AnimationState::Die;
		if (m_pMovementComp)
			m_pMovementComp->SetDisable(true);
		m_pSpriteComp->SetCurrentSprite(static_cast<int>(AnimationState::Die));
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
	if (m_IsDead || m_pPepperShooter == nullptr)
		return;

	//const auto dir = m_CurrMovementDir == MoveDirection::None ? m_LastMovementDir : m_CurrMovementDir;
	auto dir = biggin::PossessGameObjectComponent::MoveDirection::Down;
	if (m_pMovementComp)
	{
		m_pMovementComp->SetDisable(true);
		dir = m_pMovementComp->GetCurrMovementDir();
		dir = dir == biggin::PossessGameObjectComponent::MoveDirection::None
			                 ? m_pMovementComp->GetLastDirection()
			                 : dir;
	}

	if(!m_pPepperShooter->Shoot(dir))
		return;

	m_IsShooting = true;

	m_DelayedStopShooting.Reset();
	switch (dir)
	{
	case biggin::PossessGameObjectComponent::MoveDirection::Left:
		m_pSpriteComp->SetCurrentSprite(static_cast<int>(AnimationState::PepperHorizontal));
		m_pSpriteComp->SetFlip(SDL_FLIP_HORIZONTAL);
		break;
	case biggin::PossessGameObjectComponent::MoveDirection::Right:
		m_pSpriteComp->SetCurrentSprite(static_cast<int>(AnimationState::PepperHorizontal));
		break;
	case biggin::PossessGameObjectComponent::MoveDirection::Up:
	case biggin::PossessGameObjectComponent::MoveDirection::Down:
		m_pSpriteComp->SetCurrentSprite(static_cast<int>(AnimationState::PepperVertical));
		break;
	}

}

//TODO: remove
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
