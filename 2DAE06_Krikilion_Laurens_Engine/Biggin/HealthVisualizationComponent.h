#pragma once
#include "Component.h"
#include "Observer.h"

namespace biggin
{
	class TextComponent;
	class RenderComponent;

	class HealthVisualizationComponent final : public Component, public Observer
	{
	public:
		HealthVisualizationComponent(GameObject* go);

		void Start() override;

		void OnNotify(const Component* entity, const std::string& event) override;

	private:
		const GameObject* m_pOwningGameObject;
		TextComponent* m_pLivesText;
	};
}

