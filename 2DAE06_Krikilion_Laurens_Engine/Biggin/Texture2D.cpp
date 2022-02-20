#include "BigginPCH.h"
#include "Texture2D.h"
#include "SDL_render.h"
#include "Renderer.h"
#include "GameObject.h"

dae::Texture2D::~Texture2D()
{
	SDL_DestroyTexture(m_Texture);
}

void dae::Texture2D::Render(Transform sceneTransform, Transform transform) const
{
	const auto& pos = transform.GetPosition();
	const auto& scenePos = sceneTransform.GetPosition();
	Renderer::GetInstance().RenderTexture(*this, pos.x + scenePos.x, pos.y + scenePos.y);
}

SDL_Texture* dae::Texture2D::GetSDLTexture() const
{
	return m_Texture;
}

dae::Texture2D::Texture2D(SDL_Texture* texture) : m_Texture(texture)
{
}

