#pragma once
#include "Component.h"
#include "Observer.h"
#include <string>

namespace biggin
{
	class RenderComponent;
	class TextComponent;
}

namespace burgerTime
{
	class PepperUI final : public biggin::Component, public biggin::Observer
	{
	public:
		PepperUI(biggin::GameObject* go);
		void Initialize(biggin::GameObject*) override;

		void OnNotify(Component* entity, const std::string& event) override;

	private:
		biggin::TextComponent* m_pScoreText;
	};
}
