#include "BigginPCH.h"
#include "Sprite.h"

#include "ResourceManager.h"
#include "Texture2D.h"

Sprite::Sprite(std::string path, dae::Transform transform) : Component()
                                                             , m_Transform(transform)
{
	m_Texture = dae::ResourceManager::GetInstance().LoadTexture(path);
}

void Sprite::Render(dae::Transform sceneTransform) const
{
	m_Texture.get()->Render(sceneTransform, m_Transform);
}
