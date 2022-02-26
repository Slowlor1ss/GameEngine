#include "BigginPCH.h"
#include "Sprite.h"

#include "ResourceManager.h"
#include "Texture2D.h"

Sprite::Sprite(dae::GameObject* go, std::string path, dae::Transform transform) : Component(go)
                                                             , m_Transform(transform)
{
	m_Texture = dae::ResourceManager::GetInstance().LoadTexture(path);
}

Sprite::Sprite(dae::GameObject* go) : Component(go)
	,m_Transform(dae::Transform())
	,m_Texture(nullptr)
{
}

void Sprite::Render(dae::Transform sceneTransform) const
{
	m_Texture.get()->Render(sceneTransform, m_Transform);
}
