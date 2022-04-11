#pragma once
#include "Component.h"
#include "Observer.h"

namespace biggin
{
	class TextComponent;
	class RenderComponent;

	class ScoreUI final : public Component, public Observer
	{
	public:
		ScoreUI(GameObject* go);
		void Initialize(GameObject*) override;

		void OnNotify(const Component* entity, const std::string& event) override;

	private:
		TextComponent* m_pScoreText;
	};
}

