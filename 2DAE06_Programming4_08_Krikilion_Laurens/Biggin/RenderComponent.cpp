#include "BigginPCH.h"
#include "RenderComponent.h"
#include "GameObject.h"
#include "Logger.h"
#include "Renderer.h"
#include "ResourceManager.h"
#include "Transform.h"

using namespace biggin;

RenderComponent::RenderComponent(GameObject* go, std::string path, const glm::vec2& offset) : Component(go)
	, m_Transform(go->GetTransform())
	, m_Offset(offset)
{
	m_pTexture = ResourceManager::GetInstance().LoadTexture(path);
}

RenderComponent::RenderComponent(GameObject* go) : Component(go)
	,m_Transform(go->GetTransform())
	,m_Offset({0,0})
	,m_pTexture(nullptr)
{
}

void RenderComponent::Render() const
{
	if (!SDL_RectEmpty(&m_SrcRect) && !SDL_RectEmpty(&m_DstRect))
	{
		Renderer::GetInstance().RenderTexture(*m_pTexture, &m_DstRect, &m_SrcRect, m_Flip);
	}
	else
	{
		const auto& pos = m_Transform->GetLocalPosition();
		Renderer::GetInstance().RenderTexture(*m_pTexture, pos.x + m_Offset.x, pos.y + m_Offset.y);
	}
}

