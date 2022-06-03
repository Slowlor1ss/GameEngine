#include "BigginPCH.h"
#include "ScoreUI.h"
#include "GameObject.h"
#include "Logger.h"
#include "ScoreComponent.h"
#include "Subject.h"
#include "TextComponent.h"

using namespace burgerTime;

ScoreUI::ScoreUI(biggin::GameObject* go)
	: Component(go)
    , m_pScoreText(nullptr)
{
}

void ScoreUI::Initialize(biggin::GameObject* go)
{
	m_pScoreText = go->GetComponent<biggin::TextComponent>();
	if (m_pScoreText == nullptr)
		Logger::GetInstance().LogErrorAndBreak("Missing TextComponent");
}

void ScoreUI::OnNotify(biggin::Component* entity, const std::string& event)
{
	if (event != "ScoreChanged")
		return;

	const int& score = static_cast<const burgerTime::ScoreComponent*>(entity)->GetScore();
	m_pScoreText->SetText("Score: " + std::to_string(score));
}
