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

	class ScoreUI final : public biggin::Component, public biggin::Observer
	{
	public:
		ScoreUI(biggin::GameObject* go);
		void Initialize(biggin::GameObject*) override;

		void OnNotify(Component* entity, const std::string& event) override;

	private:
		biggin::TextComponent* m_pScoreText;
	};
}

