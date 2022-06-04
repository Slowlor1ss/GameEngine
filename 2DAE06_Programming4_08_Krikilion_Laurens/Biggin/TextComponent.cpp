#include "BigginPCH.h"
#include <SDL_ttf.h>
#include "TextComponent.h"
#include "Biggin.h"
#include "Renderer.h"
#include "Font.h"
#include "ResourceManager.h"
#include "GameObject.h"
#include "Logger.h"
#include "RenderComponent.h"
#include "Texture2D.h"

biggin::TextComponent::TextComponent(biggin::GameObject* go, const std::string& text, const std::shared_ptr<Font>& font) : Component(go)
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


	m_TextTexture = go->GetComponent<RenderComponent>();
	if (m_TextTexture == nullptr)
		Logger::GetInstance().LogWarning("Could't find a Sprite did you forget to add a RenderComponent?");
	else
		m_TextTexture->SetTexture(std::make_shared<Texture2D>(texture));

	m_WrapLength = Biggin::GetWindowWidth();
}

biggin::TextComponent::TextComponent(biggin::GameObject* go) : TextComponent(go, "Default Text", ResourceManager::GetInstance().LoadFont("Lingua.otf", 32))
{}

void biggin::TextComponent::Update()
{
	if (m_NeedsUpdate)
	{
		SDL_Surface* surf{};
		if (m_WrapLength > 0)
			surf = TTF_RenderText_Blended_Wrapped(m_Font->GetFont(), m_Text.c_str(), m_Color, m_WrapLength);
		else
			surf = TTF_RenderText_Blended(m_Font->GetFont(), m_Text.c_str(), m_Color);

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

// This implementation uses the "dirty flag" pattern
void biggin::TextComponent::SetText(const std::string& text)
{
	m_Text = text;
	m_NeedsUpdate = true;
}

void biggin::TextComponent::SetColor(const SDL_Color& color)
{
	m_Color = color;
}


