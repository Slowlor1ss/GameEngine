#pragma once
#include <mutex>
#include <queue>
#include "SoundSystem.h"

//very simple audio system

class SimpleSDL2SoundSystem final : public SoundSystem
{
	struct queuedAudio
	{
		std::string filename{};
		int volume{};
		bool isSoundEffect{}; //multiple sound effects can play but with music, only 1 at a time plays
	};

public:
	SimpleSDL2SoundSystem(const std::string& dataPath);
	~SimpleSDL2SoundSystem() override;

	SimpleSDL2SoundSystem(const SimpleSDL2SoundSystem& other) = delete;
	SimpleSDL2SoundSystem(SimpleSDL2SoundSystem&& other) noexcept = delete;
	SimpleSDL2SoundSystem& operator=(const SimpleSDL2SoundSystem& other) = delete;
	SimpleSDL2SoundSystem& operator=(SimpleSDL2SoundSystem&& other) noexcept = delete;

	void Init() override {};
	void Play(const std::string& filename, float volume, bool isSoundEffect = true) override;
	void StopAll() override;
	void StopMusic() override;
	void PauseAll() override;
	void UnpauseAll() override;

private:
	void ProcessQueue();

	std::string m_DataPath;
	std::queue<queuedAudio> m_AudioQueue;

	std::thread m_AudioThread;
	std::mutex m_Mtx;
	std::condition_variable m_ConditionVariable;
	bool m_Running;
};

