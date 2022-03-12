#include "BigginPCH.h"
#include "Sprite.h"
#include "GameObject.h"
#include "ResourceManager.h"
#include "Texture2D.h"

using namespace dae;

RenderComponent::RenderComponent(dae::GameObject* go, std::string path) : Component(go)
	, m_Transform(go->GetTransform())
{
	m_Texture = ResourceManager::GetInstance().LoadTexture(path);
}

RenderComponent::RenderComponent(dae::GameObject* go) : Component(go)
	,m_Transform(go->GetTransform())
	,m_Texture(nullptr)
{
}

void RenderComponent::Render() const
{
	m_Texture.get()->Render(m_Transform);
}
