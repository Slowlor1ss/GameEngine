#include "BigginPCH.h"
#include "HealthUI.h"
#include "GameObject.h"
#include "HealthComponent.h"
#include "Logger.h"
#include "TextComponent.h"

using namespace burgerTime;

HealthUI::HealthUI(biggin::GameObject* go)
	: Component(go)
	,m_pLivesText(go->GetComponent<biggin::TextComponent>())
{
}

void HealthUI::Initialize(biggin::GameObject* go)
{
	m_pLivesText = go->GetComponent<biggin::TextComponent>();
	if (m_pLivesText == nullptr)
		Logger::GetInstance().LogErrorAndBreak("Missing TextComponent");
}

void HealthUI::OnNotify(Component* entity, const std::string& event)
{
	if (event != "HealthChanged")
		return;

	const int& livesLeft = static_cast<const biggin::HealthComponent*>(entity)->GetLives();
	m_pLivesText->SetText(std::to_string(livesLeft) + " lives");
}
