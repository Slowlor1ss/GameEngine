#pragma once
#include "Component.h"

namespace biggin
{
	class Texture2D;
	class Transform;

	class RenderComponent final : public Component
	{
	public:
		RenderComponent(GameObject* go, std::string path, const glm::vec2& offset = { 0, 0 });
		RenderComponent(GameObject* go);

		void Render() const override;

		void SetTexture(const std::shared_ptr<Texture2D> texture) { m_pTexture = texture; }
		void SetSourceRect(const SDL_Rect& src) { m_SrcRect = src; };
		void SetDstRect(const SDL_Rect& dst) { m_DstRect = dst; };
		void TranslateDstRect(const SDL_Point& translation) { m_DstRect.x += translation.x; m_DstRect.y += translation.y; }
		void SetFlip(SDL_RendererFlip flip) { m_Flip = flip; };

	private:
		std::shared_ptr<Texture2D> m_pTexture{nullptr};

		GameObject* m_GameObjectRef{nullptr};
		glm::vec2 m_Offset;
		SDL_Rect m_SrcRect;
		SDL_Rect m_DstRect;
		SDL_RendererFlip m_Flip;
	};

}
