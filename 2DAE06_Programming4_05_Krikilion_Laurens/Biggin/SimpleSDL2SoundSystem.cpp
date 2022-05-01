#include "BigginPCH.h"
#include "SimpleSDL2SoundSystem.h"
#include "audio.h"

SimpleSDL2SoundSystem::SimpleSDL2SoundSystem(const std::string& dataPath)
	:m_AudioThread(&SimpleSDL2SoundSystem::ProcessQueue, this)
	,m_Running(true)
{
	m_DataPath = dataPath;
}

SimpleSDL2SoundSystem::~SimpleSDL2SoundSystem()
{
	m_Running = false;
	m_ConditionVariable.notify_one();
	m_AudioThread.join();

	/* End Simple-SDL2-Audio */
	endAudio();
}

void SimpleSDL2SoundSystem::Play(const std::string& filename, float volume, bool isSoundEffect)
{
	std::unique_lock lock{ m_Mtx };
	m_AudioQueue.emplace(m_DataPath + filename, int(volume * SDL_MIX_MAXVOLUME), isSoundEffect);
	m_ConditionVariable.notify_one();
}

void SimpleSDL2SoundSystem::StopAll()
{
	//https://stackoverflow.com/questions/709146/how-do-i-clear-the-stdqueue-efficiently
	std::queue<queuedAudio> empty;
	std::swap(m_AudioQueue, empty);

	endAudio();
}

void SimpleSDL2SoundSystem::ProcessQueue()
{
	std::unique_lock lock{ m_Mtx };
	lock.unlock();

	while (m_Running)
	{
		lock.lock();
		if (!m_AudioQueue.empty())
		{
			const queuedAudio audio = m_AudioQueue.front();
			m_AudioQueue.pop();
			lock.unlock();

			if (audio.isSoundEffect)
				playSound(audio.filename.c_str(), audio.volume);
			else
				playMusic(audio.filename.c_str(), audio.volume);
		}
		else
		{
			//unlocks lock, then sleeps
			m_ConditionVariable.wait(lock); //wait until woken up
			//enter lock

			lock.unlock();
		}
	}
}
