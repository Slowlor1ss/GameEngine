#include "BigginPCH.h"
#include "FpsCounter.h"
#include "GameObject.h"
#include "GameTime.h"
#include "Scene.h"
#include "TextComponent.h"

using namespace biggin;

FpsCounter::FpsCounter(biggin::GameObject* go) : Component(go)
{
    m_pFpsText = go->GetComponent<biggin::TextComponent>();
}

void FpsCounter::Update()
{
    m_pFpsText->SetText(std::to_string(GameTime::GetInstance().GetFPS()));
}

void FpsCounter::Render() const
{
}