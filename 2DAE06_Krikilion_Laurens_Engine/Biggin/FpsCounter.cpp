#include "BigginPCH.h"
#include "FpsCounter.h"

#include "GameTime.h"
#include "Scene.h"
#include "TextComponent.h"

FpsCounter::FpsCounter(dae::GameObject* go) : Component(go)
	//, m_pFpsText{new dae::TextComponent}
{
    //m_pFpsText->SetPosition(10, 10); //TODO: clean this up
    //m_pFpsText->SetColor({0, 255, 0, 1});
    m_pFpsText = m_pGameObject->GetComponent<dae::TextComponent>();
}

FpsCounter::~FpsCounter()
{
    //SafeDelete(m_pFpsText);
}

void FpsCounter::Update()
{
    m_pFpsText->SetText(std::to_string(GameTime::GetInstance().GetFPS()));
}

void FpsCounter::Render(dae::Transform ) const //TODO: remove this gameobjects will take car of this same thing for above maybe
{
    //m_pFpsText->Render(sceneTransform);
}
//auto FPSfont = ResourceManager::GetInstance().LoadFont("Lingua.otf", 36);
//auto FPSto = std::make_shared<TextObject>(to_string(scene.GetSceneContext().GetGameTime()->GetFPS()), font);