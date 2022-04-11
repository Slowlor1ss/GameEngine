#include "BigginPCH.h"
#include "HealthUI.h"
#include "GameObject.h"
#include "HealthComponent.h"
#include "Logger.h"
#include "PeterPepper.h"
#include "Subject.h"
#include "TextComponent.h"

using namespace biggin;

HealthUI::HealthUI(GameObject* go) : Component(go)
	,m_pLivesText(go->GetComponent<TextComponent>())
{
}

void biggin::HealthUI::Initialize(GameObject* go)
{
	m_pLivesText = go->GetComponent<TextComponent>();
	if (m_pLivesText == nullptr)
		Logger::GetInstance().LogErrorAndBreak("Missing TextComponent");
}

void HealthUI::OnNotify(const Component* entity, const std::string& event)
{
	if (event != "HealthChanged")
		return;

	const int& livesLeft = static_cast<const HealthComponent*>(entity)->GetLives();
	m_pLivesText->SetText(std::to_string(livesLeft) + " lives");
}
