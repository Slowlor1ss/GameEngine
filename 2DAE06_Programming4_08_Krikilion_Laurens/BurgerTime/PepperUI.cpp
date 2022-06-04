#include "PepperUI.h"
#include "ScoreUI.h"
#include "GameObject.h"
#include "Logger.h"
#include "PepperShooter.h"
#include "TextComponent.h"

using namespace burgerTime;

PepperUI::PepperUI(biggin::GameObject* go)
	: Component(go)
	, m_pScoreText(nullptr)
{
}

void PepperUI::Initialize(biggin::GameObject* go)
{
	m_pScoreText = go->GetComponent<biggin::TextComponent>();
	if (m_pScoreText == nullptr)
		Logger::GetInstance().LogErrorAndBreak("Missing TextComponent");

}

void PepperUI::OnNotify(biggin::Component* entity, const std::string& event)
{
	if (event != "PepperChanged")
		return;

	const int& pepper = static_cast<const PepperShooter*>(entity)->GetAmntPepper();
	m_pScoreText->SetText("Pepper:\n" + std::to_string(pepper));
}