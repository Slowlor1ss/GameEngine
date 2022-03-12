#include "BigginPCH.h"
#include "Texture2D.h"
#include "SDL_render.h"
#include "Renderer.h"
#include "Transform.h"

dae::Texture2D::~Texture2D()
{
	SDL_DestroyTexture(m_Texture);
}

void dae::Texture2D::Render(const Transform* transform) const
{
	const auto& pos = transform->GetPosition();
	Renderer::GetInstance().RenderTexture(*this, pos.x, pos.y);
}

SDL_Texture* dae::Texture2D::GetSDLTexture() const
{
	return m_Texture;
}

dae::Texture2D::Texture2D(SDL_Texture* texture) : m_Texture(texture)
{
}

