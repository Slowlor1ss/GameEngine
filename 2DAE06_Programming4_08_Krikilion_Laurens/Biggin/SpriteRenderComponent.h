#pragma once
#include <unordered_map>
#include "Component.h"
#include <SDL.h>

namespace biggin
{
	class RenderComponent;
	class Texture2D;

	struct AtlasPart
	{
		AtlasPart() = default;
		AtlasPart(int columns, const SDL_Point& pos, const SDL_Point& sizeCell)
			: columns(columns), pos(pos), sizeCell(sizeCell)
		{}

		int columns{};
		SDL_Point pos{};
		SDL_Point sizeCell{};
	};

	struct SpriteInfo
	{
		SpriteInfo() = default;
		SpriteInfo(int frameCount, int startFrameIdx)
			: startFrameIdx(startFrameIdx), frameCount(frameCount)
		{
		};

		unsigned int startFrameIdx{};
		unsigned int frameCount{};
		auto operator<=>(const SpriteInfo&) const = default;
	};

	class SpriteRenderComponent : public biggin::Component
	{
	public:
		SpriteRenderComponent(GameObject* go, const AtlasPart& atlasPart, float speed = 0.5f);
		~SpriteRenderComponent() override = default;

		SpriteRenderComponent(const SpriteRenderComponent& other) = delete;
		SpriteRenderComponent(SpriteRenderComponent&& other) noexcept = delete;
		SpriteRenderComponent& operator=(const SpriteRenderComponent& other) = delete;
		SpriteRenderComponent& operator=(SpriteRenderComponent&& other) noexcept = delete;

		void Update() override;
		void Initialize(GameObject*) override;

		void AddAnimation(int animationState, const SpriteInfo& spriteInfo);
		void SetCurrentSprite(int animationState);
		int GetCurrentSprite() const { return m_CurrentSpriteIdx; };
		void SetPause(bool pause) { m_IsPaused = pause; } //TODO: add looping option
		void SetFinishAndPause() { m_PausedWhenFinished = true; }
		void SetFlip(SDL_RendererFlip flip) const;
		void UpdateRenderVariables();

		float GetDuration(int animationState);
		float GetDurationCurrentSprite();

	private:
		std::pair<unsigned int, unsigned int> GetRowCol(const unsigned int idx, const unsigned int nrOfCols);

		AtlasPart m_AtlasPart;
		std::unordered_map<int, SpriteInfo> m_Sprites{};
		SpriteInfo m_CurrentSprite;

		RenderComponent* m_RenderComponent;
		SDL_Rect m_SourceRect{};
		SDL_Rect m_DstRect{};

		bool m_IsPaused;
		bool m_PausedWhenFinished{};
		bool m_RenderVariablesDirty{};
		unsigned int m_CurrentRelFrameIdx;
		int m_CurrentSpriteIdx{};
		float m_Speed;//TODO: maybe do speed in sprite info
		float m_ElapsedSec;
	};
}

