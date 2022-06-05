#pragma once
#include "Component.h"
#include "Observer.h"

namespace biggin
{
	class RenderComponent;
	class TextComponent;
}

namespace burgerTime
{

	class HighScoreUI final : public biggin::Component, public biggin::Observer
	{
	public:
		HighScoreUI(biggin::GameObject* go);
		void Initialize(biggin::GameObject*) override;

		void OnNotify(Component* entity, const std::string& event) override;

	private:
		biggin::TextComponent* m_pScoreText;
	};
}

