#pragma once
#include "Singleton.h"
#pragma warning(push, 0)
#include <glm/glm.hpp>
#include <SDL.h>
#pragma warning (pop)

struct b2Vec2;

namespace biggin
{
	class Transform;
	class Texture2D;
	/**
	 * Simple RAII wrapper for the SDL renderer
	 */
	class Renderer final : public Singleton<Renderer>
	{
		SDL_Renderer* m_Renderer{};
		SDL_Window* m_Window{};
		SDL_Color m_clearColor{};	
	public:
		void Init(SDL_Window* window);
		void Render() const;
		void Destroy();

		void RenderTexture(const Texture2D& texture, float x, float y) const;
		void RenderTexture(const Texture2D& texture, float x, float y, float width, float height, SDL_RendererFlip flip = SDL_FLIP_NONE) const;
		void RenderTexture(const Texture2D& texture, const glm::vec2& pos, const glm::vec2& sizeCell, SDL_RendererFlip flip = SDL_FLIP_NONE) const;
		void RenderTexture(const Texture2D& texture, const SDL_Rect* dst, const SDL_Rect* src, SDL_RendererFlip flip) const;

		void RenderPolygon(std::vector<glm::vec2> points, SDL_Color color, bool closed = true) const;
		void RenderPolygon(const glm::vec2* points, int vertexCount, SDL_Color color, bool closed = true) const;
		void RenderPoint(glm::vec2 point, SDL_Color color = {255, 0, 0, 1}) const;

		void RenderCircle(glm::vec2 point, int32_t radius, SDL_Color color = { 255, 0, 0, 1 }) const;

		SDL_Renderer* GetSDLRenderer() const { return m_Renderer; }

		const SDL_Color& GetBackgroundColor() const { return m_clearColor; }
		void SetBackgroundColor(const SDL_Color& color) { m_clearColor = color; }
	};
}

