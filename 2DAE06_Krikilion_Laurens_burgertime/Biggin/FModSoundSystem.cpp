#include "BigginPCH.h"
#include "FModSoundSystem.h"
#include <ranges>

void Implementation::Update()
{
    std::vector<ChannelMap::iterator> stoppedChannels;
    for (auto it = m_Channels.begin(), itEnd = m_Channels.end(); it != itEnd; ++it)
    {
        bool isPlaying = false;
        it->second->isPlaying(&isPlaying);
        if (!isPlaying)
        {
            stoppedChannels.push_back(it);
        }
    }
    for (const auto& stoppedChannel : stoppedChannels)
    {
        m_Channels.erase(stoppedChannel);
    }
    m_System->update();
}

void FmodSoundSystem::Init()
{
    m_SoundEngineImpl = new Implementation;
}

void FmodSoundSystem::Update()
{
    m_SoundEngineImpl->Update();
}

void FmodSoundSystem::Shutdown()
{
    delete m_SoundEngineImpl;
}

int FmodSoundSystem::ErrorCheck(FMOD_RESULT result)
{
#ifdef _DEBUG
    if (result != FMOD_OK) 
    {
	    std::cout << "FMOD ERROR " << result << '\n';
        return 1;
    }
#endif

    return 0;
}

void FmodSoundSystem::LoadSound(const std::string& soundName, bool isLooping, bool isStreaming)
{
    auto foundIt = m_SoundEngineImpl->m_Sounds.find(soundName);
    if (foundIt != m_SoundEngineImpl->m_Sounds.end())
        return;

    FMOD_MODE mode = FMOD_NONBLOCKING;
    mode |= false ? (FMOD_3D | FMOD_3D_INVERSETAPEREDROLLOFF) : FMOD_2D; // Later add 3D
    mode |= isLooping ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
    mode |= isStreaming ? FMOD_CREATESTREAM : FMOD_CREATECOMPRESSEDSAMPLE;

    FMOD::Sound* pSound = nullptr;
    m_SoundEngineImpl->m_System->createSound(soundName.c_str(), mode, nullptr,
											&pSound);
    if (pSound)
    {
        m_SoundEngineImpl->m_Sounds[soundName] = pSound;
	}
}

void FmodSoundSystem::UnloadSound(const std::string& soundName)
{
	auto foundIt = m_SoundEngineImpl->m_Sounds.find(soundName);
	if (foundIt == m_SoundEngineImpl->m_Sounds.end())
		return;
    foundIt->second->release();
    m_SoundEngineImpl->m_Sounds.erase(foundIt);
}

void FmodSoundSystem::Set2dListenerAndOrientation(const glm::vec2& pos, const glm::vec2& vLook)
{
    //TODO: FMOD listenener
}

int FmodSoundSystem::PlaySound(const std::string& soundName, const glm::vec2& pos, float volumedB)
{
	const int nChannelId = m_SoundEngineImpl->m_NextChannelid++;
    auto foundIt = m_SoundEngineImpl->m_Sounds.find(soundName);
    if (foundIt == m_SoundEngineImpl->m_Sounds.end())
    {
        LoadSound(soundName);
        foundIt = m_SoundEngineImpl->m_Sounds.find(soundName);
        if (foundIt == m_SoundEngineImpl->m_Sounds.end())
        {
            return nChannelId;
        }
    }
    FMOD::Channel* pChannel = nullptr;
    m_SoundEngineImpl->m_System->playSound(foundIt->second, nullptr, true, &pChannel);
    if(pChannel)
    {
        FMOD_VECTOR position = VectorToFmod(pos);
        pChannel->set3DAttributes(&position, nullptr);
        pChannel->setVolume(dbToVolume(volumedB));
        pChannel->setPaused(false);
        m_SoundEngineImpl->m_Channels[nChannelId] = pChannel;
    }
    return nChannelId;
}

void FmodSoundSystem::StopChannel(int nChannelId)
{
    auto foundIt = m_SoundEngineImpl->m_Channels.find(nChannelId);
    if (foundIt == m_SoundEngineImpl->m_Channels.end())
        return;

    foundIt->second->stop();
}

void FmodSoundSystem::StopAllChannels()
{
    //also works as v or just use .second 
    //for (const auto& channel : m_SoundEngineImpl->m_Channels | std::views::values)
    for (const auto& channel : std::views::values(m_SoundEngineImpl->m_Channels))
    {
        channel->stop();
    }
}

void FmodSoundSystem::SetChannel2dPosition(int nChannelId, const glm::vec2& pos)
{
    auto foundIt = m_SoundEngineImpl->m_Channels.find(nChannelId);
    if (foundIt == m_SoundEngineImpl->m_Channels.end())
        return;

    FMOD_VECTOR position = VectorToFmod(pos);
    foundIt->second->set3DAttributes(&position, nullptr);
}

void FmodSoundSystem::SetChannelVolume(int nChannelId, float volumedB)
{
    auto foundIt = m_SoundEngineImpl->m_Channels.find(nChannelId);
    if (foundIt == m_SoundEngineImpl->m_Channels.end())
        return;

    foundIt->second->setVolume(dbToVolume(volumedB));
}

bool FmodSoundSystem::IsPlaying(int nChannelId) const
{
    auto foundIt = m_SoundEngineImpl->m_Channels.find(nChannelId);
    if (foundIt == m_SoundEngineImpl->m_Channels.end())
        return false;

    bool retVal;
    foundIt->second->isPlaying(&retVal);
    return retVal;
}

void FmodSoundSystem::LoadBank(const std::string& strBankName, FMOD_STUDIO_LOAD_BANK_FLAGS flags)
{
    auto tFoundIt = m_SoundEngineImpl->m_Banks.find(strBankName);
    if (tFoundIt != m_SoundEngineImpl->m_Banks.end())
        return;
    FMOD::Studio::Bank* pBank;
    ErrorCheck(m_SoundEngineImpl->m_StudioSystem->loadBankFile(strBankName.c_str(), flags, &pBank));
    if (pBank) 
    {
        m_SoundEngineImpl->m_Banks[strBankName] = pBank;
    }
}

void FmodSoundSystem::LoadEvent(const std::string& eventName)
{
    auto foundit = m_SoundEngineImpl->m_Events.find(eventName);
    if (foundit != m_SoundEngineImpl->m_Events.end())
        return;
    FMOD::Studio::EventDescription* pEventDescription = nullptr;
    ErrorCheck(m_SoundEngineImpl->m_StudioSystem->getEvent(eventName.c_str(), &pEventDescription));
    if (pEventDescription) {
        FMOD::Studio::EventInstance* pEventInstance = nullptr;
        ErrorCheck(pEventDescription->createInstance(&pEventInstance));
        if (pEventInstance) {
            m_SoundEngineImpl->m_Events[eventName] = pEventInstance;
        }
    }
}

void FmodSoundSystem::PlayEvent(const std::string& eventName)
{
    auto foundit = m_SoundEngineImpl->m_Events.find(eventName);
    if (foundit == m_SoundEngineImpl->m_Events.end()) {
        LoadEvent(eventName);
        foundit = m_SoundEngineImpl->m_Events.find(eventName);
        if (foundit == m_SoundEngineImpl->m_Events.end())
            return;
    }
    foundit->second->start();
}

void FmodSoundSystem::StopEvent(const std::string& eventName, bool immediate)
{
	auto foundIt = m_SoundEngineImpl->m_Events.find(eventName);
    if (foundIt == m_SoundEngineImpl->m_Events.end())
        return;
    FMOD_STUDIO_STOP_MODE eMode;
    eMode = immediate ? FMOD_STUDIO_STOP_IMMEDIATE : FMOD_STUDIO_STOP_ALLOWFADEOUT;
    ErrorCheck(foundIt->second->stop(eMode));
}

void FmodSoundSystem::GetEventParameter(const std::string& eventName, const std::string& parameterName, float* parameter)
{
    auto foundIt = m_SoundEngineImpl->m_Events.find(eventName);
    if (foundIt == m_SoundEngineImpl->m_Events.end())
        return;

    ErrorCheck(foundIt->second->getParameterByName(parameterName.c_str(), parameter));
}

void FmodSoundSystem::SetEventParameter(const std::string& eventName, const std::string& parameterName, float value)
{
    auto foundIt = m_SoundEngineImpl->m_Events.find(eventName);
    if (foundIt == m_SoundEngineImpl->m_Events.end())
        return;

    ErrorCheck(foundIt->second->setParameterByName(parameterName.c_str(), value));
}

bool FmodSoundSystem::IsEventPlaying(const std::string& eventName) const
{
    auto foundIt = m_SoundEngineImpl->m_Events.find(eventName);
    if (foundIt == m_SoundEngineImpl->m_Events.end())
        return false;

    FMOD_STUDIO_PLAYBACK_STATE* state = nullptr;
    if (foundIt->second->getPlaybackState(state) == FMOD_STUDIO_PLAYBACK_PLAYING) {
        return true;
    }
    return false;
}
