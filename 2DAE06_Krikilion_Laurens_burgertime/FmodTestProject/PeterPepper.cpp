#include "PeterPepper.h"
#include "BigginPCH.h"
#include "PeterPepper.h"
#include "BoxCollider2d.h"
#include "GameObject.h"
#include "GameTime.h"
#include "InputManager.h"
#include "Logger.h"
#include "PossessGameObjectComponent.h"
#include "SoundServiceLocator.h"
#include "SpriteRenderComponent.h"

character::PeterPepper::PeterPepper(biggin::GameObject* go)
	: Component(go)
	, m_GameTimeRef(biggin::GameTime::GetInstance())
	, m_pGameObjectRef(go)
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

}
