#pragma once
#include "Component.h"

namespace biggin
{
	class RenderComponent;
	class Font;
	class Texture2D;

	class TextComponent final : public Component
	{
	public:
		explicit TextComponent(biggin::GameObject* go, const std::string& text, const std::shared_ptr<Font>& font);
		explicit TextComponent(biggin::GameObject* go);
		~TextComponent() override = default;

		TextComponent(const TextComponent& other) = delete;
		TextComponent(TextComponent&& other) = delete;
		TextComponent& operator=(const TextComponent& other) = delete;
		TextComponent& operator=(TextComponent&& other) = delete;

		void Start() override {};
		void Update() override;
		void FixedUpdate() override;
		void Render() const override;
		void RenderUi() override {};

		void SetText(const std::string& text);
		void SetColor(const SDL_Color& color);

	private:
		bool m_NeedsUpdate;
		std::string m_Text;
		SDL_Color m_Color{255,255,255,1};
		std::shared_ptr<Font> m_Font{nullptr};
		RenderComponent* m_TextTexture{nullptr};
	};
}
