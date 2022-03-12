#include "BigginPCH.h"
#include "FpsCounter.h"
#include "GameObject.h"
#include "GameTime.h"
#include "Scene.h"
#include "TextComponent.h"

FpsCounter::FpsCounter(dae::GameObject* go) : Component(go)
{
    m_pFpsText = m_pGameObject->GetComponent<dae::TextComponent>();
}

void FpsCounter::Update()
{
    m_pFpsText->SetText(std::to_string(GameTime::GetInstance().GetFPS()));
}

void FpsCounter::Render() const
{
}