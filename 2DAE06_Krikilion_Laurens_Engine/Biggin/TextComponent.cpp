#include "BigginPCH.h"
#include <SDL_ttf.h>
#include "TextComponent.h"
#include "Renderer.h"
#include "Font.h"
#include "ResourceManager.h"
#include "GameObject.h"
#include "Logger.h"
#include "Sprite.h"
#include "Texture2D.h"

dae::TextComponent::TextComponent(dae::GameObject* go, const std::string& text, const std::shared_ptr<Font>& font) : Component(go)
	, m_NeedsUpdate(true), m_Text(text), m_Font(font)
{
	const auto surf = TTF_RenderText_Blended(m_Font->GetFont(), m_Text.c_str(), m_Color);
	if (surf == nullptr)
	{
		throw std::runtime_error(std::string("Render text failed: ") + SDL_GetError());
	}
	auto texture = SDL_CreateTextureFromSurface(Renderer::GetInstance().GetSDLRenderer(), surf);
	if (texture == nullptr)
	{
		throw std::runtime_error(std::string("Create text texture from surface failed: ") + SDL_GetError());
	}
	SDL_FreeSurface(surf);

	m_TextTexture = go->GetComponent<Sprite>();
	if (m_TextTexture == nullptr)
	{
		Logger::GetInstance().LogWarning("Could't find a Sprite did you forget to add a spritecomponent");
	}
	else
	{
		m_TextTexture->SetTexture(std::make_shared<Texture2D>(texture));
	}

}

dae::TextComponent::TextComponent(dae::GameObject* go) : TextComponent(go, "Default Text", ResourceManager::GetInstance().LoadFont("Lingua.otf", 36))
{}

void dae::TextComponent::Update()
{
	if (m_NeedsUpdate)
	{
		const auto surf = TTF_RenderText_Blended(m_Font->GetFont(), m_Text.c_str(), m_Color);
		if (surf == nullptr) 
		{
			throw std::runtime_error(std::string("Render text failed: ") + SDL_GetError());
		}
		auto texture = SDL_CreateTextureFromSurface(Renderer::GetInstance().GetSDLRenderer(), surf);
		if (texture == nullptr) 
		{
			throw std::runtime_error(std::string("Create text texture from surface failed: ") + SDL_GetError());
		}
		SDL_FreeSurface(surf);
		m_TextTexture->SetTexture(std::make_shared<Texture2D>(texture));
		m_NeedsUpdate = false;
	}
}

void dae::TextComponent::FixedUpdate()
{
}

void dae::TextComponent::Render(Transform ) const
{
	//if (m_TextTexture != nullptr)
	//{
	//	const auto& pos = m_Transform.GetPosition();
	//	const auto& scenePos = sceneTransform.GetPosition();
	//	Renderer::GetInstance().RenderTexture(*m_TextTexture, pos.x + scenePos.x, pos.y + scenePos.y);
	//}
}

// This implementation uses the "dirty flag" pattern
void dae::TextComponent::SetText(const std::string& text)
{
	m_Text = text;
	m_NeedsUpdate = true;
}

void dae::TextComponent::SetColor(const SDL_Color& color)
{
	m_Color = color;
}

void dae::TextComponent::SetPosition(const float x, const float y)
{
	m_TextTexture->SetPos(x, y, 0.0f);
}


