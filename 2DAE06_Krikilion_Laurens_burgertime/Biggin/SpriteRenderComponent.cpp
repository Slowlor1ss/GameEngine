#include "BigginPCH.h"
#include <iostream>
#include <sstream>
#include "SpriteRenderComponent.h"
#include "GameObject.h"
#include "GameTime.h"
#include "Logger.h"
#include "RenderComponent.h"

biggin::SpriteRenderComponent::SpriteRenderComponent(GameObject* go, const AtlasPart& atlasPart, float speed) : Component(go)
	, m_AtlasPart(atlasPart)
	, m_RenderComponent(nullptr)
	, m_IsPaused(false)
	, m_CurrentRelFrameIdx(0)
	, m_Speed(speed)
	, m_ElapsedSec(0)
	, m_GameObjectRef(go)
{
	const auto& [widthCell, heightCell] = m_AtlasPart.sizeCell;

	m_DstRect.w = widthCell;
	m_DstRect.h = heightCell;
}

void biggin::SpriteRenderComponent::Initialize(GameObject* go)
{
	m_RenderComponent = go->GetComponent<RenderComponent>();
	if (m_RenderComponent == nullptr)
		Logger::GetInstance().LogErrorAndBreak("Missing RenderComponent");
	//Set source rect to something very small to prevent showing the whole sprite sheet on the first frame
	m_RenderComponent->SetSourceRect({ 1,1,1,1 });
}

void biggin::SpriteRenderComponent::AddAnimation(int animationState, const SpriteInfo& spriteInfo)
{
	m_Sprites[animationState] = spriteInfo;
}

void biggin::SpriteRenderComponent::SetCurrentSprite(int animationState)
{
	if (!m_Sprites.contains(animationState))
	{
		std::cerr << "Theres no sprite animation for index: " << animationState << '\n';
		return;
	}

	const auto &nextSprite = m_Sprites[animationState];
	if (m_CurrentSprite == nextSprite)
		return;

	m_PausedWhenFinished = false;
	m_CurrentSpriteIdx = animationState;
	m_CurrentSprite = nextSprite;
	m_CurrentRelFrameIdx = 0;
	m_ElapsedSec = 0;
	m_RenderVariablesDirty = true;
}

void biggin::SpriteRenderComponent::SetFlip(SDL_RendererFlip flip) const
{
	if (m_RenderComponent == nullptr)
		Logger::GetInstance().LogWarning("Render component has not been set yet when set flip was called");
	else
		m_RenderComponent->SetFlip(flip);
}

void biggin::SpriteRenderComponent::UpdateRenderVariables()
{
	const auto& gameObjectPos = m_GameObjectRef->GetLocalPosition();
	const auto& [widthCell, heightCell] = m_AtlasPart.sizeCell;

	//center rect
	m_DstRect.x = static_cast<int>(gameObjectPos.x - widthCell * 0.5f);
	m_DstRect.y = static_cast<int>(gameObjectPos.y - heightCell * 0.5f);
	m_RenderComponent->SetDstRect(m_DstRect);

	const auto& partPos = m_AtlasPart.pos;
	const auto [row, col]{ GetRowCol(m_CurrentSprite.startFrameIdx + m_CurrentRelFrameIdx, m_AtlasPart.columns) };

	m_SourceRect.x = partPos.x + col * m_DstRect.w;
	m_SourceRect.y = partPos.y + row * m_DstRect.h;
	m_SourceRect.w = widthCell;
	m_SourceRect.h = heightCell;
	m_RenderComponent->SetSourceRect(m_SourceRect);
}

float biggin::SpriteRenderComponent::GetDuration(int animationState)
{
	if (!m_Sprites.contains(animationState))
	{
		std::cerr << "Theres no sprite animation for index: " << animationState << '\n';
		return 0;
	}

	const auto& animation = m_Sprites[animationState];
	return animation.frameCount * m_Speed;
}

float biggin::SpriteRenderComponent::GetDurationCurrentSprite()
{
	return GetDuration(m_CurrentSpriteIdx);
}

void biggin::SpriteRenderComponent::Update()
{
	if (m_RenderVariablesDirty)
	{
		UpdateRenderVariables();
	}

	m_ElapsedSec += GameTime::GetInstance().GetDeltaT();

	if (m_IsPaused || m_CurrentSprite.frameCount == 0) return;

	//Update the frame that's shown
	if (m_ElapsedSec >= m_Speed) 
	{
		if (m_CurrentRelFrameIdx == m_CurrentSprite.frameCount-1)
		{
			m_IsPaused = m_PausedWhenFinished;
			if(m_IsPaused)
				m_PausedWhenFinished = false;
			else
				m_CurrentRelFrameIdx = 0;
		}
		else
			++m_CurrentRelFrameIdx;

		m_RenderVariablesDirty = true;
		m_ElapsedSec = 0;
	}
}

std::pair<unsigned int, unsigned int> biggin::SpriteRenderComponent::GetRowCol(const unsigned int idx, const unsigned int nrOfCols)
{
	const unsigned int j = idx / nrOfCols;
	const unsigned int i = idx - (j * nrOfCols);
	return { j, i };
}
