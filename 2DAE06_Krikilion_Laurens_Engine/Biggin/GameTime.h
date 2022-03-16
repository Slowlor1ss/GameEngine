#pragma once
#include <chrono>

#include "Singleton.h"

//Based on PhysXFramework

class GameTime final : public biggin::Singleton<GameTime>
{
public:
	~GameTime() override = default;

	GameTime(const GameTime& other) = delete;
	GameTime(GameTime&& other) noexcept = delete;
	GameTime& operator=(const GameTime& other) = delete;
	GameTime& operator=(GameTime&& other) noexcept = delete;

	void Reset();
	void Update();

	float GetDeltaT() const { return  m_DeltaTime; }
	float GetTotal() const { return m_TotalGameTime; }
	int GetFPS() const { return m_FPS; }
	static constexpr float GetFixedTimeStep() { return m_FixedTimeStep; }
	bool IsRunning() const { return !m_IsPaused; }

	void Start();
	void Stop();

private:
	GameTime() = default;
	friend Singleton<GameTime>;

	float m_TotalGameTime{};
	float m_DeltaTime{};

	static constexpr float m_FixedTimeStep{ 0.02f }; //20ms

	std::chrono::high_resolution_clock::time_point m_BaseTime{};
	std::chrono::high_resolution_clock::time_point m_StopTime{};
	std::chrono::high_resolution_clock::time_point m_PrevTime{ std::chrono::steady_clock::now() };
	std::chrono::high_resolution_clock::time_point m_CurrTime{};
	std::chrono::high_resolution_clock::duration m_PausedDuration{};

	bool m_IsPaused{ false };

	int m_FPS{};
	float m_FpsTimer{};
	int m_FpsCount{};
};

