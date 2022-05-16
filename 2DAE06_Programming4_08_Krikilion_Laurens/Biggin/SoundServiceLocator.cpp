#include "BigginPCH.h"
#include "SoundServiceLocator.h"

NullSoundSystem SoundServiceLocator::m_DefaultSoundSystem;
SoundSystem* SoundServiceLocator::m_pSoundInstance = &m_DefaultSoundSystem;

SoundSystem& SoundServiceLocator::GetSoundSystem()
{
	return *m_pSoundInstance;
}

void SoundServiceLocator::RegisterSoundSystem(SoundSystem* ss)
{
	//prevents dangling pointer
	if (m_pSoundInstance != &m_DefaultSoundSystem)
		delete m_pSoundInstance;
	m_pSoundInstance = ss == nullptr ? &m_DefaultSoundSystem : ss;
}
