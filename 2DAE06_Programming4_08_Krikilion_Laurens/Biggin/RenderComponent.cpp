#include "BigginPCH.h"
#include "RenderComponent.h"
#include "GameObject.h"
#include "Logger.h"
#include "Renderer.h"
#include "ResourceManager.h"
#include "Transform.h"

using namespace biggin;

RenderComponent::RenderComponent(GameObject* go, const std::string& path, const glm::vec2& offset)
	: Component(go)
	, m_GameObjectRef(go)
	, m_Offset(offset)
{
	m_pTexture = ResourceManager::GetInstance().LoadTexture(path);
}

RenderComponent::RenderComponent(GameObject* go)
	: Component(go)
	, m_pTexture(nullptr)
	, m_GameObjectRef(go)
	, m_Offset({0,0})
{
}

void RenderComponent::Render() const
{
	if (!SDL_RectEmpty(&m_SrcRect) && !SDL_RectEmpty(&m_DstRect))
	{
		Renderer::GetInstance().RenderTexture(*m_pTexture, &m_DstRect, &m_SrcRect, m_Flip);
	}
	else if(!SDL_RectEmpty(&m_SrcRect))
	{
		const auto& pos = m_GameObjectRef->GetWorldPosition();
		const SDL_Rect dstRect{ static_cast<int>(pos.x + m_Offset.x), static_cast<int>(pos.y + m_Offset.y) , m_SrcRect.w, m_SrcRect.h};
		Renderer::GetInstance().RenderTexture(*m_pTexture, &dstRect, &m_SrcRect, m_Flip);
	}
	else
	{
		const auto& pos = m_GameObjectRef->GetWorldPosition();
		Renderer::GetInstance().RenderTexture(*m_pTexture, pos.x + m_Offset.x, pos.y + m_Offset.y);
	}
}

void biggin::RenderComponent::SetTexture(const std::shared_ptr<Texture2D> texture)
{
	m_pTexture = texture;
}

void biggin::RenderComponent::SetTexture(const std::string& path)
{
	m_pTexture = ResourceManager::GetInstance().LoadTexture(path);
}

void biggin::RenderComponent::SetSourceRect(const SDL_Rect& src)
{
	m_SrcRect = src;
}

void biggin::RenderComponent::SetDstRect(const SDL_Rect& dst)
{
	m_DstRect = dst;
}

void biggin::RenderComponent::TranslateDstRect(const SDL_Point& translation)
{
	m_DstRect.x += translation.x;
	m_DstRect.y += translation.y;
}

