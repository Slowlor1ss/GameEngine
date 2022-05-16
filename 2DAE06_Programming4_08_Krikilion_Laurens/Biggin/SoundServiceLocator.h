#pragma once
#include "NullSoundSystem.hpp"
#include "SoundSystem.h"

class SoundServiceLocator final
{
	static SoundSystem* m_pSoundInstance;
	static NullSoundSystem m_DefaultSoundSystem;

public:
	SoundServiceLocator() = delete;

	static SoundSystem& GetSoundSystem();
	static void RegisterSoundSystem(SoundSystem* ss);

	static void Cleanup() { delete m_pSoundInstance; m_pSoundInstance = nullptr; }
};

