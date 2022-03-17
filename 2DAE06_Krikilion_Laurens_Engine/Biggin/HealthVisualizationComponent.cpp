#include "BigginPCH.h"
#include "HealthVisualizationComponent.h"
#include "GameObject.h"
#include "HealthComponent.h"
#include "PeterPepper.h"
#include "Subject.h"
#include "TextComponent.h"

using namespace biggin;

HealthVisualizationComponent::HealthVisualizationComponent(GameObject* go) : Component(go)
	,m_pOwningGameObject(go)
	,m_pLivesText(go->GetComponent<TextComponent>())
{
}

void biggin::HealthVisualizationComponent::Start()
{
	//TODO: add safety checks
	const auto subject = m_pOwningGameObject->GetParent()->GetComponent<Subject>();
	subject->AddObserver(this);
}

void HealthVisualizationComponent::OnNotify(const Component* entity, const std::string& event)
{
	if (event != "HealthChanged")
		return;

	const int& livesLeft = static_cast<const HealthComponent*>(entity)->GetLives();
	m_pLivesText->SetText(std::to_string(livesLeft) + " lives");
}
