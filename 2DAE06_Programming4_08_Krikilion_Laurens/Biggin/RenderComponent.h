#pragma once
#include "Component.h"
#include <SDL.h>

namespace biggin
{
	class Texture2D;
	class Transform;

	class RenderComponent final : public Component
	{
	public:
		RenderComponent(GameObject* go, const std::string& path, const glm::vec2& offset = { 0, 0 });
		RenderComponent(GameObject* go);

		void Render() const override;

		void SetTexture(const std::shared_ptr<Texture2D> texture);
		void SetTexture(const std::string& path);
		void SetSourceRect(const SDL_Rect& src);
		void SetDstRect(const SDL_Rect& dst);
		void TranslateDstRect(const SDL_Point& translation);
		void SetOffset(const glm::vec2& offset) { m_Offset = offset; };
		void SetFlip(SDL_RendererFlip flip) { m_Flip = flip; };

	private:
		std::shared_ptr<Texture2D> m_pTexture{nullptr};

		GameObject* m_GameObjectRef{nullptr};
		glm::vec2 m_Offset{};
		SDL_Rect m_SrcRect{};
		SDL_Rect m_DstRect{};
		SDL_RendererFlip m_Flip;
	};

}
