#include "BigginPCH.h"
#include "Box2dRenderer.h"
#include "Renderer.h"

void Box2dRenderer::DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
	biggin::Renderer::GetInstance().RenderPolygon(reinterpret_cast<const glm::vec2*>(vertices), vertexCount,
		SDL_Color{ static_cast<uint8_t>(color.r*255), static_cast<uint8_t>(color.g*255), static_cast<uint8_t>(color.b*255), static_cast<uint8_t>(color.a) });
};
