#include "BigginPCH.h"
#include "GameTime.h"

void GameTime::Reset()
{
	std::chrono::steady_clock::time_point currTime{ std::chrono::steady_clock::now() };
	m_BaseTime = currTime;
	m_PrevTime = currTime;
	m_StopTime = {};
	m_FpsTimer = {};
	m_FpsCount = {};
	m_IsPaused = false;
}

void GameTime::Update()
{
	//if (m_IsPaused)
	//{
	//	m_FPS = 0;
	//	m_DeltaTime = 0.0f;

	//	//calc time game is running - paused time
	//	std::chrono::high_resolution_clock::duration baseTillNow{ m_CurrTime - m_BaseTime };
	//	std::chrono::high_resolution_clock::duration totalGameTime{ baseTillNow - m_PausedDuration };
	//	m_TotalGameTime = std::chrono::duration<float>(totalGameTime).count();

	//	return;
	//}

	//m_CurrTime = std::chrono::steady_clock::now();

	//m_DeltaTime = std::chrono::duration<float>(m_CurrTime - m_PrevTime).count();

	//m_PrevTime = m_CurrTime;

	//// avoid neg time
	//if (m_DeltaTime < 0.0f)
	//	m_DeltaTime = 0.0f;

	////calc time game is running - paused time
	//std::chrono::high_resolution_clock::duration baseTillNow{ m_CurrTime - m_BaseTime };
	//std::chrono::high_resolution_clock::duration totalGameTime{ baseTillNow - m_PausedDuration };
	//m_TotalGameTime = std::chrono::duration<float>(totalGameTime).count();

	////FPS LOGIC
	////m_FpsTimer += m_DeltaTime;
	////++m_FpsCount;
	////if (m_FpsTimer >= 1.0f)
	////{
	////	m_FPS = m_FpsCount;
	////	m_FpsCount = 0;
	////	m_FpsTimer -= 1.0f;
	////}

	//m_FPS = int(1.f / m_DeltaTime);
	//std::cout << m_FPS << '\n';



	if (m_IsPaused)
	{
		m_FPS = 0;
		m_DeltaTime = 0.0f;

		//calc time game is running - paused time
		std::chrono::high_resolution_clock::duration baseTillNow{ m_CurrTime - m_BaseTime };
		std::chrono::high_resolution_clock::duration totalGameTime{ baseTillNow - m_PausedDuration };
		m_TotalGameTime = std::chrono::duration<float>(totalGameTime).count();

		return;
	}

	m_CurrTime = std::chrono::steady_clock::now();

	m_DeltaTime = std::chrono::duration<float>(m_CurrTime - m_PrevTime).count();

	m_PrevTime = m_CurrTime;

	// avoid neg time
	if (m_DeltaTime < 0.0f)
		m_DeltaTime = 0.0f;

	//calc time game is running - pauzed time
	std::chrono::high_resolution_clock::duration baseTillNow{ m_CurrTime - m_BaseTime };
	std::chrono::high_resolution_clock::duration totalGameTime{ baseTillNow - m_PausedDuration };
	m_TotalGameTime = std::chrono::duration<float>(totalGameTime).count();

	//FPS LOGIC
	m_FpsTimer += m_DeltaTime;
	++m_FpsCount;
	//Update every second
	if (m_FpsTimer >= 1.0f)
	{
		m_FPS = m_FpsCount;
		m_FpsCount = 0;
		m_FpsTimer -= 1.0f;
	}
}

void GameTime::Start()
{
	if (m_IsPaused)
	{
		std::chrono::steady_clock::time_point startTime{ std::chrono::steady_clock::now() };
		m_PausedDuration += (startTime - m_StopTime);

		m_PrevTime = startTime;
		m_StopTime = {};
		m_IsPaused = false;
	}
}

void GameTime::Stop()
{
	if (!m_IsPaused)
	{
		std::chrono::steady_clock::time_point currTime{ std::chrono::steady_clock::now() };
		m_StopTime = currTime;
		m_IsPaused = true;
	}
}
