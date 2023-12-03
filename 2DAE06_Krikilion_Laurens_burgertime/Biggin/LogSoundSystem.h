#pragma once
#include "SoundSystem.h"

class Logger;

class LogSoundSystem final : public SoundSystem
{
public:
	LogSoundSystem(SoundSystem* systemToLog);
	~LogSoundSystem() override;

	LogSoundSystem(const LogSoundSystem& other) = delete;
	LogSoundSystem(LogSoundSystem&& other) noexcept = delete;
	LogSoundSystem& operator=(const LogSoundSystem& other) = delete;
	LogSoundSystem& operator=(LogSoundSystem&& other) noexcept = delete;

	void Init() override {};
	void Play(const std::string& filename, float volume, bool isSoundEffect) override;
	void StopAll() override;
	void StopMusic() override;
	void PauseAll() override;
	void UnpauseAll() override;

private:
	SoundSystem* m_System;
};

