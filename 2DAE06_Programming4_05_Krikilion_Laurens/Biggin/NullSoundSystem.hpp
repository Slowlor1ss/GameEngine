#pragma once
#include "SoundSystem.h"

class NullSoundSystem final : public SoundSystem
{
public:
	void Play(const std::string& /*filename*/, float /*volume*/, bool /*isSoundEffect*/ = true) override {}
	void StopAll() override {}
};

