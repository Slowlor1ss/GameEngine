#include "BigginPCH.h"
#include "FModSoundSystem.h"
#include <ranges>
#include <algorithm>

void AudioFader::StartFade(float toVolumedB, float fadeTimeSeconds)
{
    FromVolumedB = GetCurrentVolumedB();
    ToVolumedB = toVolumedB;
    CurrentTime = 0.0f;
    TimeFade = fadeTimeSeconds;
}

void AudioFader::StartFade(float fromVolumedB, float toVolumedB, float fadeTimeSeconds)
{
    FromVolumedB = fromVolumedB;
    ToVolumedB = toVolumedB;
    CurrentTime = 0.0f;
    TimeFade = fadeTimeSeconds;
}

void AudioFader::Update(float deltaTime)
{
    CurrentTime += deltaTime;
}

float AudioFader::GetCurrentVolumedB() const {
    return std::lerp(FromVolumedB, ToVolumedB, std::clamp(CurrentTime / TimeFade, 0.0f, 1.0f));
}

float AudioFader::GetCurrentVolume() const {
    return FmodSoundSystem::dBToVolume(GetCurrentVolumedB());
}


Channel::Channel(FmodCoreEngine& engine, TypeId soundId, const SoundDefinition& definition, const Vector3& position, float volumedB)
    : m_Engine(engine), m_Channel(nullptr), m_SoundId(soundId), m_Position(position), m_VolumedB(volumedB), m_SoundVolume(Helper::dBToVolume(volumedB))
{
    auto soundIt = m_Engine.Sounds.find(m_SoundId);
    if (soundIt == m_Engine.Sounds.end()) return;

    m_Engine.System->playSound(soundIt->second->m_Sound, nullptr, true, &m_Channel);
    if (m_Channel)
    {
        UpdateChannelParameters();
        m_Channel->setPaused(false);
    }
}

Channel::~Channel()
{
}

void Channel::Update(float deltaTime)
{
    switch (m_State)
    {
    case State::INITIALIZE: [[fallthrough]];
    case State::DEVIRTUALIZE:
    case State::TOPLAY:
    {
        if (m_StopRequested)
        {
            m_State = State::STOPPING;
            return;
        }

        if (ShouldBeVirtual(true))
        {
            if (IsOneShot())
            {
                m_State = State::STOPPING;
            }
            else
            {
                m_State = State::VIRTUAL;
            }
            return;
        }

        if (!m_Engine.SoundIsLoaded(m_SoundId))
        {
            m_Engine.LoadSound(m_SoundId);
            m_State = State::LOADING;
            return;
        }

        m_Channel = nullptr;
        auto soundIt = m_Engine.Sounds.find(m_SoundId);
        if (soundIt != m_Engine.Sounds.end())
        {
            m_Engine.System->playSound(soundIt->second->m_Sound, nullptr, true, &m_Channel);
            if (m_Channel)
            {
                if (m_State == State::DEVIRTUALIZE)
                {
                    //Fade In for Virtualize
                    m_VirtualizeFader.StartFade(SILENCE_dB, 0.0f, VIRTUALIZE_FADE_TIME);
                }
                m_State = State::PLAYING;

                FMOD_VECTOR p = FmodSoundSystem::VectorToFmod(m_Pos);
                m_Channel->set3DAttributes(&p, nullptr);
                m_Channel->setVolume(FmodSoundSystem::dBToVolume(m_VolumedB));
                m_Channel->setPaused(false);
            }
            else
            {
                m_State = State::STOPPING;
            }
        }

        break;
    }
    case State::LOADING:
    {
        if (m_Engine.SoundIsLoaded(m_SoundId))
        {
            m_State = State::TOPLAY;
        }
        break;
    }
    case State::PLAYING:
    {
        m_VirtualizeFader.Update(deltaTime);
        // Update everything, the position, the volume, everything...
        UpdateChannelParameters();

        if (!IsPlaying() || m_StopRequested)
        {
            m_State = State::Stopping;
            return;
        }

        if (ShouldBeVirtual(false))
        {
            m_VirtualizeFader.StartFade(SILENCE_dB, VIRTUALIZE_FADE_TIME);
            m_State = State::VIRTUALIZING;
        }
        break;
    }
    case State::STOPPING:
    {
        m_StopFader.Update(deltaTime);
        UpdateChannelParameters();
        if (m_StopFader.IsFinished())
        {
            m_Channel->stop();
        }
        if (!IsPlaying())
        {
            m_State = State::STOPPED;
            return;
        }
        break;
    }
    case State::STOPPED:
    {
        break;
    }
    case State::VIRTUALIZING:
    {
        m_VirtualizeFader.Update(deltaTime);
        UpdateChannelParameters();
        if (ShouldBeVirtual(false))
        {
            m_VirtualizeFader.StartFade(0.0f, VIRTUALIZE_FADE_TIME);
            m_State = State::PLAYING;
        }
        if (m_VirtualizeFader.IsFinished())
        {
            m_Channel->stop();
            m_State = State::VIRTUAL;
        }
        break;
    }
    case State::VIRTUAL:
    {
        if (m_StopRequested)
        {
            m_State = State::STOPPING;
        }
        else if (!ShouldBeVirtual(false))
        {
            m_State = State::DEVIRTUALIZE;
        }
        break;
    }
    }
}

float Channel::GetVolumedB() const {
    return m_VolumedB;
}

bool Channel::IsPlaying() const {
    bool isPlaying = false;
    if (m_Channel == nullptr) return isPlaying;

    m_Channel->isPlaying(&isPlaying);
    return isPlaying;
}

void Channel::UpdateChannelParameters()
{
    if (m_Channel == nullptr) return;

    FMOD_VECTOR p = FmodSoundSystem::VectorToFmod(m_Position);
    m_Channel->set3DAttributes(&p, nullptr);
    m_Channel->setVolume(FmodSoundSystem::dBToVolume(m_VolumedB));
}

bool Channel::IsOneShot() const
{
    auto soundIt = m_Engine.Sounds.find(m_SoundId);
    if (soundIt == m_Engine.Sounds.end()) return false;

    return soundIt->second->m_OneShot;
}

// It should be some calculation to see if the sound is still worth playing.
// Maybe a lookup in the sound defintion to see if the sound is worth virtualizing.
// Most likely a check of the distance of the sound so when it's above the specified threshold we virtualize it.
// Currently, only a distance check is done.
bool Channel::ShouldBeVirtual(bool allowVirtualOneShot) const
{
    if (!allowVirtualOneShot && IsOneShot()) return false;

    auto defPtr = GetSoundDefinition();
    if (!defPtr) return false;

    FMOD_VECTOR pos;
    FMOD_VECTOR vel;
    FMOD_VECTOR forward;
    FMOD_VECTOR up;
    m_Engine.System->get3DListenerAttributes(0, &pos, &vel, &forward, &up);

    Vector3 listenerPos = FmodHelper::FmodToVector(pos);

    float distance = glm::distance(m_Position, listenerPos);

    return distance > defPtr->maxDistance;
}


Implementation::Implementation(): m_NextChannelid(0)
{
	m_StudioSystem = nullptr;
	FmodSoundSystem::ErrorCheck(FMOD::Studio::System::create(&m_StudioSystem));
	FmodSoundSystem::ErrorCheck(
		m_StudioSystem->initialize(32, FMOD_STUDIO_INIT_LIVEUPDATE, FMOD_INIT_PROFILE_ENABLE, NULL));

	m_System = nullptr;
	FmodSoundSystem::ErrorCheck(m_StudioSystem->getCoreSystem(&m_System));
}

Implementation::~Implementation() {
    FmodSoundSystem::ErrorCheck(m_StudioSystem->unloadAll());
    FmodSoundSystem::ErrorCheck(m_StudioSystem->release());
}


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

int FmodSoundSystem::ErrorCheck(FMOD_RESULT result [[maybe_unused]])
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

void FmodSoundSystem::LoadSound(int soundId)
{
    if (IsSoundLoaded(soundId))
        return;

    auto foundIt = m_SoundEngineImpl->m_Sounds.find(soundId);
    if (foundIt != m_SoundEngineImpl->m_Sounds.end())
        return false;

    FMOD_MODE mode = FMOD_NONBLOCKING;
    mode |= false ? (FMOD_3D | FMOD_3D_INVERSETAPEREDROLLOFF) : FMOD_2D; // Later add 3D
    mode |= isLooping ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
    mode |= isStreaming ? FMOD_CREATESTREAM : FMOD_CREATECOMPRESSEDSAMPLE;

    FMOD::Sound* pSound = nullptr;
    ErrorCheck(m_SoundEngineImpl->m_System->createSound(soundName.c_str(), mode, nullptr,
											&pSound));
    if (pSound)
    {
        m_SoundEngineImpl->m_Sounds[soundName] = pSound;
	}
}

bool FmodSoundSystem::IsSoundReady(const std::string& soundName)
{
    auto foundIt = m_SoundEngineImpl->m_Sounds.find(soundName);
    if (foundIt == m_SoundEngineImpl->m_Sounds.end())
        return false;

    // Check if the sound is fully loaded
    FMOD_OPENSTATE openState;
    ErrorCheck(foundIt->second->getOpenState(&openState, nullptr, nullptr, nullptr));

    return(openState == FMOD_OPENSTATE_READY);
}

void FmodSoundSystem::UnloadSound(const std::string& soundName)
{
	auto foundIt = m_SoundEngineImpl->m_Sounds.find(soundName);
	if (foundIt == m_SoundEngineImpl->m_Sounds.end())
		return;
    foundIt->second->release();
    m_SoundEngineImpl->m_Sounds.erase(foundIt);
}

void FmodSoundSystem::Set2dListenerAndOrientation(const glm::vec2&, const glm::vec2&)
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
    ErrorCheck(m_SoundEngineImpl->m_System->playSound(foundIt->second, nullptr, true, &pChannel));
    if(pChannel)
    {
        FMOD_VECTOR position = VectorToFmod(pos);
        pChannel->set3DAttributes(&position, nullptr);
        pChannel->setVolume(dBToVolume(volumedB));
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

    foundIt->second->setVolume(dBToVolume(volumedB));
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
    if (static_cast<FMOD_STUDIO_PLAYBACK_STATE>(foundIt->second->getPlaybackState(state)) == FMOD_STUDIO_PLAYBACK_PLAYING) {
        return true;
    }
    return false;
}
