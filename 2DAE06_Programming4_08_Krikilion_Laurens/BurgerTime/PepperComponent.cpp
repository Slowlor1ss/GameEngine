#include "PepperComponent.h"
#include "GameObject.h"
#include "GameTime.h"
#include "Logger.h"
#include "Scene.h"
#include "SpriteRenderComponent.h"

burgerTime::PepperComponent::PepperComponent(biggin::GameObject* go)
	: Component(go)
	, m_GameTimeRef(biggin::GameTime::GetInstance())
{
}

void burgerTime::PepperComponent::Initialize(biggin::GameObject* go)
{
	m_pSpriteComp = go->GetComponent<biggin::SpriteRenderComponent>();
	if (m_pSpriteComp == nullptr)
		Logger::GetInstance().LogErrorAndBreak("Missing SpriteRenderComponent");

	m_DelayedRemove.interval = m_pSpriteComp->GetDuration(0);
	m_DelayedRemove.func = [&go] {go->GetSceneRef()->Remove(go); };
}

void burgerTime::PepperComponent::Update()
{
	m_DelayedRemove.Update(m_GameTimeRef.GetDeltaT());
}
