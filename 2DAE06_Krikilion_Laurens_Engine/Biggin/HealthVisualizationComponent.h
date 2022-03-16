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
		void Start() override {};
		void Update() override {};
		void FixedUpdate() override {};
		void Render() const override;
		void RenderUi() override {};

	private:
		std::shared_ptr<TextComponent> m_pLivesText;
	};
}

