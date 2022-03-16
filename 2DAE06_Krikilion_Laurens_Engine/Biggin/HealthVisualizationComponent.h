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
		HealthVisualizationComponent(biggin::GameObject* go);

		void OnNotify(const Component* entity, const std::string& event) override;

	private:
		std::shared_ptr<TextComponent> m_pLivesText;
	};
}

