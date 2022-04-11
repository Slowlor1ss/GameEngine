#include "BigginPCH.h"
#include "ScoreUI.h"
#include "GameObject.h"
#include "Logger.h"
#include "ScoreComponent.h"
#include "Subject.h"
#include "TextComponent.h"

biggin::ScoreUI::ScoreUI(GameObject* go) : Component(go)
	, m_pScoreText(nullptr)
{
}

void biggin::ScoreUI::Initialize(GameObject* go)
{
	m_pScoreText = go->GetComponent<TextComponent>();
	if (m_pScoreText == nullptr)
		Logger::GetInstance().LogErrorAndBreak("Missing TextComponent");
}

void biggin::ScoreUI::OnNotify(const Component* entity, const std::string& event)
{
	if (event != "ScoreChanged")
		return;

	const int& score = static_cast<const ScoreComponent*>(entity)->GetScore();
	m_pScoreText->SetText("Score: " + std::to_string(score));
}
