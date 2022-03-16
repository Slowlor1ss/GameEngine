#include "BigginPCH.h"
#include "Texture2D.h"
#include "SDL_render.h"

biggin::Texture2D::~Texture2D()
{
	SDL_DestroyTexture(m_Texture);
}

SDL_Texture* biggin::Texture2D::GetSDLTexture() const
{
	return m_Texture;
}

biggin::Texture2D::Texture2D(SDL_Texture* texture) : m_Texture(texture)
{
}

