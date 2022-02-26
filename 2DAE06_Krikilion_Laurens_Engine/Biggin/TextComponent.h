#pragma once
#include "Component.h"
#include "GameObject.h"
#include "GameObject.h"
#include "Transform.h"

class Sprite;

namespace dae
{
	class Font;
	class Texture2D;
	class TextComponent final : public Component
	{
	public:
		explicit TextComponent(dae::GameObject* go, const std::string& text, const std::shared_ptr<Font>& font);
		explicit TextComponent(dae::GameObject* go);
		~TextComponent() override = default;

		TextComponent(const TextComponent& other) = delete;
		TextComponent(TextComponent&& other) = delete;
		TextComponent& operator=(const TextComponent& other) = delete;
		TextComponent& operator=(TextComponent&& other) = delete;

		void Update() override;
		void FixedUpdate() override;
		void Render(Transform sceneTransform) const override;

		void SetText(const std::string& text);
		void SetColor(const SDL_Color& color);
		void SetPosition(float x, float y);

	private:
		bool m_NeedsUpdate;
		std::string m_Text;
		SDL_Color m_Color{255,255,255,1};
		//Transform m_Transform;
		std::shared_ptr<Font> m_Font{nullptr};
		std::shared_ptr<Sprite> m_TextTexture{nullptr};
	};
}
