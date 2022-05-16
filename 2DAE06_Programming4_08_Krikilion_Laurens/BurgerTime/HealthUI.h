#pragma once
#include "Component.h"
#include "Observer.h"

namespace biggin
{
	class TextComponent;
	class RenderComponent;

	class HealthUI final : public Component, public Observer
	{
	public:
		HealthUI(GameObject* go);
		void Initialize(GameObject* go) override;

		void OnNotify(Component* entity, const std::string& event) override;

	private:
		TextComponent* m_pLivesText;
	};
}

