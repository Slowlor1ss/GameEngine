#include "BigginPCH.h"
#include "HighScoreUI.h"
#include "GameObject.h"
#include "Logger.h"
#include "ScoreComponent.h"
#include "Subject.h"
#include "TextComponent.h"

using namespace burgerTime;

HighScoreUI::HighScoreUI(biggin::GameObject* go)
	: Component(go)
	, m_pScoreText(nullptr)
{
}

void HighScoreUI::Initialize(biggin::GameObject* go)
{
	m_pScoreText = go->GetComponent<biggin::TextComponent>();
	if (m_pScoreText == nullptr)
		Logger::GetInstance().LogErrorAndBreak("Missing TextComponent");
}

void HighScoreUI::OnNotify(biggin::Component* entity, const std::string& event)
{
	if (event != "HighScoreChanged")
		return;

	const auto& scores = static_cast<const burgerTime::ScoreComponent*>(entity)->GetHighScores();
	std::string highscoreText{"HIGHSCORES:\n"};

	for (int value : scores)
	{
		highscoreText += std::to_string(value);
		highscoreText += '\n';
	}

	m_pScoreText->SetText(highscoreText);
}
