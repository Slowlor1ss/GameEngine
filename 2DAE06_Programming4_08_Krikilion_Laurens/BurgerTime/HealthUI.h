#pragma once
#include "Component.h"
#include "Observer.h"

namespace biggin
{
	class TextComponent;
	class RenderComponent;
}

namespace burgerTime
{
	class HealthUI final : public biggin::Component, public biggin::Observer
	{
	public:
		HealthUI(biggin::GameObject* go);
		void Initialize(biggin::GameObject* go) override;

		void OnNotify(Component* entity, const std::string& event) override;

	private:
		biggin::TextComponent* m_pLivesText;
	};
}

