#include "BigginPCH.h"
#include "HealthVisualizationComponent.h"
#include "GameObject.h"
#include "HealthComponent.h"
#include "TextComponent.h"

using namespace biggin;

HealthVisualizationComponent::HealthVisualizationComponent(biggin::GameObject* go) : Component(go)
	,m_pLivesText(go->GetComponent<TextComponent>())
{
}

void HealthVisualizationComponent::OnNotify(const Component* entity, const std::string& event)
{
	if (event != "HealthChanged")
		return;

	const int& livesLeft = static_cast<const HealthComponent*>(entity)->GetLives();
	m_pLivesText->SetText(std::to_string(livesLeft) + " lives");
}
