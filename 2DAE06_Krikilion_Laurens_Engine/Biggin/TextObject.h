#pragma once
#include "Component.h"
#include "GameObject.h"
#include "GameObject.h"
#include "Transform.h"

namespace dae
{
	class Font;
	class Texture2D;
	class TextObject final : public Component
	{
	public:

		explicit TextObject(const std::string& text, const std::shared_ptr<Font>& font);
		explicit TextObject();
		~TextObject() override = default;

		TextObject(const TextObject& other) = delete;
		TextObject(TextObject&& other) = delete;
		TextObject& operator=(const TextObject& other) = delete;
		TextObject& operator=(TextObject&& other) = delete;

		void Update() override;
		void FixedUpdate(float) override;
		void Render(Transform sceneTransform) const override;

		void SetText(const std::string& text);
		void SetColor(const SDL_Color& color);
		void SetPosition(float x, float y);

	private:
		bool m_NeedsUpdate;
		std::string m_Text;
		SDL_Color m_Color{255,255,255,1};
		Transform m_Transform;
		std::shared_ptr<Font> m_Font{nullptr};
		std::shared_ptr<Texture2D> m_TextTexture{nullptr};
	};
}
