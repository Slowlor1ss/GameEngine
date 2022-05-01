#include "BigginPCH.h"
#include "LogSoundSystem.h"
#include "Logger.h"

LogSoundSystem::LogSoundSystem(SoundSystem* systemToLog)
	: m_System(systemToLog)
{
	std::cout << "Created sound system\n";
}

LogSoundSystem::~LogSoundSystem()
{
	delete m_System;
	m_System = nullptr;
	std::cout << "destroyed sound system\n";
}

void LogSoundSystem::Play(const std::string& filename, float volume, bool isSoundEffect)
{
	m_System->Play(filename, volume, isSoundEffect);
	if (isSoundEffect)
		std::cout << "playing sound: " + filename + '\n';
	else
		std::cout << "playing music: " + filename + '\n';
}

void LogSoundSystem::StopAll()
{
	std::cout << "Stopped all sound\n";
}
