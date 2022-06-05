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

character::PeterPepper::PeterPepper(biggin::GameObject* go)
	: Component(go)
	, m_IsDead(false)
	, m_IsShooting(false)
	, m_DelayedStopShooting(0.3f, [this] {m_IsShooting = false; if(m_pMovementComp) 	m_pMovementComp->SetDisable(false); }, 1, true)
	, m_GameTimeRef(biggin::GameTime::GetInstance())
	, m_pGameObjectRef(go)
	, m_pHealthComp(nullptr)
	, m_pPepperShooter(nullptr)
	, m_pSpriteComp(nullptr)
	, m_pMovementComp(nullptr)
	, m_pNotifier(go->GetComponent<biggin::Subject>())
{
	if (m_pNotifier == nullptr)
		Logger::GetInstance().LogErrorAndBreak("Missing Subject Component");
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
}

void character::PeterPepper::RespawnPlayer(const glm::vec2& pos)
{
	m_IsDead = false;
	if (m_pMovementComp)
		m_pMovementComp->SetDisable(false);

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
		dir = m_pMovementComp->GetCurrMovementDir();

		dir = dir == biggin::PossessGameObjectComponent::MoveDirection::None
			                 ? m_pMovementComp->GetLastDirection()
			                 : dir;
	}

	if(!m_pPepperShooter->Shoot(dir))
		return;

	if (m_pMovementComp)
		m_pMovementComp->SetDisable(true);

	m_IsShooting = true;

	m_DelayedStopShooting.Reset();
	switch (dir)
	{
	case biggin::PossessGameObjectComponent::MoveDirection::Left:
		m_pSpriteComp->SetCurrentSprite(static_cast<int>(AnimationState::PepperHorizontal));
		break;
	case biggin::PossessGameObjectComponent::MoveDirection::Right:
		m_pSpriteComp->SetCurrentSprite(static_cast<int>(AnimationState::PepperHorizontal));
		m_pSpriteComp->SetFlip(SDL_FLIP_HORIZONTAL);
		break;
	case biggin::PossessGameObjectComponent::MoveDirection::Up:
	case biggin::PossessGameObjectComponent::MoveDirection::Down:
		m_pSpriteComp->SetCurrentSprite(static_cast<int>(AnimationState::PepperVertical));
		break;
	}

}
