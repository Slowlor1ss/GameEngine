#include "BigginPCH.h"
#include "PossessGameObjectComponent.h"
#include <algorithm>
#include "GameObject.h"
#include "GameTime.h"
#include "Logger.h"
#include "SpriteRenderComponent.h"

biggin::PossessGameObjectComponent::PossessGameObjectComponent(GameObject* go, float maxSpeed)
	:Component(go)
	,m_pGameObjectRef(go)
	,m_MaxSpeed(maxSpeed)
{
}

void biggin::PossessGameObjectComponent::Initialize(GameObject* go)
{
	m_pSpriteComp = go->GetComponent<SpriteRenderComponent>();
	if (m_pSpriteComp == nullptr)
		Logger::GetInstance().LogWarning("Missing SpriteRenderComponent");
}

void biggin::PossessGameObjectComponent::FixedUpdate()
{
	if(m_IsDisabled)
		return;

	UpdateMovementDirectionState();

	const int nextSprite = m_DirAnimMap[m_CurrMovementDir];
	if (nextSprite != -1 && m_pSpriteComp)
	{
		m_pSpriteComp->SetCurrentSprite(nextSprite);
		m_pSpriteComp->SetFlip(m_CurrMovementDir == MoveDirection::Right ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
	}

	if (m_CurrMovementDir == MoveDirection::None)
		return;


	m_pGameObjectRef->TranslateLocalPosition(m_Velocity * GameTime::GetFixedTimeStep());

	m_Velocity = { 0,0 };
}

void biggin::PossessGameObjectComponent::UpdateMovementDirectionState()
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

	if (m_CurrMovementDir != MoveDirection::None)
		m_LastDirection = m_CurrMovementDir;

	m_CurrMovementDir = MoveDirection::None;
}


void biggin::PossessGameObjectComponent::SetVelocity(const glm::vec2& velocity)
{
	m_Velocity = glm::vec2{ std::clamp(velocity.x, -m_MaxSpeed, m_MaxSpeed), std::clamp(velocity.y, -m_MaxSpeed, m_MaxSpeed) };
}
