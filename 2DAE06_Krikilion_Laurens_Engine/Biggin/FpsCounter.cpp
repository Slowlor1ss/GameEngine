#include "BigginPCH.h"
#include "FpsCounter.h"

#include "GameTime.h"
#include "Scene.h"
#include "TextObject.h"

FpsCounter::FpsCounter(dae::Scene& scene) : Component()
                                           , m_pFpsText{new dae::TextObject}
                                           , m_Scene(scene)
{
    m_pFpsText->SetPosition(10, 10);
    m_pFpsText->SetColor({0, 255, 0, 1});
}

FpsCounter::~FpsCounter()
{
    SafeDelete(m_pFpsText);
}

void FpsCounter::Update()
{
    m_pFpsText->SetText(std::to_string(m_Scene.GetSceneContext().GetGameTime()->GetFPS()));
    m_pFpsText->Update();
}

void FpsCounter::Render(dae::Transform sceneTransform) const
{
    m_pFpsText->Render(sceneTransform);
}
//auto FPSfont = ResourceManager::GetInstance().LoadFont("Lingua.otf", 36);
//auto FPSto = std::make_shared<TextObject>(to_string(scene.GetSceneContext().GetGameTime()->GetFPS()), font);