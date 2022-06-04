#pragma once
#include "SoundSystem.h"
#include "Logger.h"

class NullSoundSystem final : public SoundSystem
{
public:
	void Play(const std::string& /*filename*/, float /*volume*/, bool /*isSoundEffect*/ = true) override { Logger::GetInstance().LogWarning("Tried to use NullSoundSystem"); }
	void StopAll() override { Logger::GetInstance().LogWarning("Tried to use NullSoundSystem"); }
	void PauseAll() override { Logger::GetInstance().LogWarning("Tried to use NullSoundSystem"); };
	void UnpauseAll() override { Logger::GetInstance().LogWarning("Tried to use NullSoundSystem"); };
};

