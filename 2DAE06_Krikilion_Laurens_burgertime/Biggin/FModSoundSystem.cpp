#include "BigginPCH.h"
#include "FModSoundSystem.h"
#include <ranges>
#include <algorithm>

AudioFader::AudioFader(Implementation::Channel* channel) : m_ChannelRef(channel)
{
}

void AudioFader::StartFade(float toVolumedB, float fadeTimeSeconds)
{
	FromVolumedB = GetCurrentVolume();
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
	m_ChannelRef->m_VolumedB = GetCurrentVolumedB();
	CurrentTime += deltaTime;
}

bool AudioFader::IsFinished() const
{
	return CurrentTime >= TimeFade;
}

float AudioFader::GetCurrentVolumedB() const {
	return std::lerp(FromVolumedB, ToVolumedB, std::clamp(CurrentTime / TimeFade, 0.0f, 1.0f));
}

float AudioFader::GetCurrentVolume() const {
	return FmodSoundSystem::dBToVolume(GetCurrentVolumedB());
}

Implementation::Sound::Sound(const FmodSoundSystem::SoundDefinition& soundDefinition, bool oneShot)
	: m_Sound(nullptr), m_SoundDefinition(soundDefinition), m_OneShot(oneShot)
{
}

Implementation::Channel::Channel(Implementation& implementation, idx soundId, 
	const FmodSoundSystem::SoundDefinition& soundDefinition [[maybe_unused]], const glm::vec2& pos, float volumedB)
	: m_EngineImpl(implementation)
	, m_Channel(nullptr)
	, m_SoundId(soundId)
	, m_Pos(pos)
	, m_VolumedB(volumedB)
	, m_OrigVolumedB(volumedB)
	, m_StopRequested(false)
	, m_VirtualizeFader(new AudioFader{ this })
	, m_StopFader(new AudioFader{ this })
{
	auto soundIt = m_EngineImpl.m_Sounds.find(m_SoundId);
	if (soundIt == m_EngineImpl.m_Sounds.end())
		return; // Maybe notify something that creation failed?

	//soundIt->second->m_SoundDefinition = soundDefinition;

	m_EngineImpl.m_System->playSound(soundIt->second->m_Sound, nullptr, true, &m_Channel); //TODO: do we want to play here already, I think so see play sound function
	if (m_Channel)
	{
		UpdateChannelParameters();
		m_Channel->setPaused(false);
	}
}

Implementation::Channel::~Channel()
{
	delete m_VirtualizeFader;
	delete m_StopFader;
	//TODO: Do we have to remove channle from the channle map and realese it? see Implementation::Update
}

void Implementation::Channel::Update(float deltaTime)
{
	switch (m_State)
	{
	case State::INITIALIZE: [[fallthrough]];
	case State::DEVIRTUALIZE:
	case State::TOPLAY:
	{
		// First check if we should bother playing the sound at all
		if (m_StopRequested)
		{
			m_State = State::STOPPING;
			return;
		}

		if (ShouldBeVirtual(true))
		{
			if (IsOneShot())
				m_State = State::STOPPING;
			else
				m_State = State::VIRTUAL;
			return;
		}

		if (!m_EngineImpl.IsSoundLoaded(m_SoundId))
		{
			m_EngineImpl.LoadSound(m_SoundId);
			m_State = State::LOADING;
			return;
		}

		m_Channel = nullptr;

		auto soundIt = m_EngineImpl.m_Sounds.find(m_SoundId);
		if (soundIt != m_EngineImpl.m_Sounds.end())
		{
			m_EngineImpl.m_System->playSound(soundIt->second->m_Sound, nullptr, true, &m_Channel);
			if (m_Channel)
			{
				if (m_State == State::DEVIRTUALIZE)
				{
					//Fade In for Virtualize
					m_VirtualizeFader->StartFade(SILENCE_dB, m_OrigVolumedB, VIRTUALIZE_FADE_TIME);
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
		if (m_EngineImpl.IsSoundLoaded(m_SoundId))
		{
			m_State = State::TOPLAY;
		}
		break;
	}
	case State::PLAYING:
	{
		// Update everything, the position, the volume, everything...
		m_VirtualizeFader->Update(deltaTime);
		UpdateChannelParameters();

		if (!IsPlaying() || m_StopRequested)
		{
			m_State = State::STOPPING;
			return;
		}

		if (ShouldBeVirtual(false))
		{
			m_VirtualizeFader->StartFade(m_VolumedB, SILENCE_dB, VIRTUALIZE_FADE_TIME);
			m_State = State::VIRTUALIZING;
		}
		break;
	}
	case State::STOPPING:
	{
		m_StopFader->Update(deltaTime);
		// Keep updating parameters as there could be a long fadeout or sound is still moving around etc etc...
		UpdateChannelParameters();
		if (m_StopFader->IsFinished())
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
		m_VirtualizeFader->Update(deltaTime);
		UpdateChannelParameters();
		if (!ShouldBeVirtual(false))
		{
			m_VirtualizeFader->StartFade(0.0f, VIRTUALIZE_FADE_TIME);
			m_State = State::PLAYING;
			break;
		}
		if (m_VirtualizeFader->IsFinished())
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

float Implementation::Channel::GetVolumedB() const {
	return m_VolumedB;
}

bool Implementation::Channel::IsPlaying() const {
	bool isPlaying = false;
	if (m_Channel == nullptr) return isPlaying;

	m_Channel->isPlaying(&isPlaying);
	return isPlaying;
}

void Implementation::Channel::UpdateChannelParameters()
{
	if (m_Channel == nullptr) return;

	FMOD_VECTOR p = FmodSoundSystem::VectorToFmod(m_Pos);
	m_Channel->set3DAttributes(&p, nullptr);
	std::cout << FmodSoundSystem::dBToVolume(m_VolumedB) << "\n";
	m_Channel->setVolume(FmodSoundSystem::dBToVolume(m_VolumedB));
	//m_Channel->setVolume(1);
}

bool Implementation::Channel::IsOneShot() const
{
	auto soundIt = m_EngineImpl.m_Sounds.find(m_SoundId);
	if (soundIt == m_EngineImpl.m_Sounds.end()) return false;

	return soundIt->second->m_OneShot;
}

const FmodSoundSystem::SoundDefinition* Implementation::Channel::GetSoundDefinition() const
{
	auto soundIt = m_EngineImpl.m_Sounds.find(m_SoundId);
	if (soundIt == m_EngineImpl.m_Sounds.end())
		return nullptr;

	return &soundIt->second->m_SoundDefinition;
}

// It should be some calculation to see if the sound is still worth playing.
// Maybe a lookup in the sound defintion to see if the sound is worth virtualizing.
// Most likely a check of the distance of the sound so when it's above the specified threshold we virtualize it.
// Currently, only a distance check is done.
bool Implementation::Channel::ShouldBeVirtual(bool allowVirtualOneShot) const
{
	if (!allowVirtualOneShot && IsOneShot()) return false;

	auto defPtr = GetSoundDefinition();
	if (!defPtr) return false;

	FMOD_VECTOR pos;
	FMOD_VECTOR vel;
	FMOD_VECTOR forward;
	FMOD_VECTOR up;
	m_EngineImpl.m_System->get3DListenerAttributes(0, &pos, &vel, &forward, &up);

	glm::vec2 listenerPos = FmodSoundSystem::FmodToVector(pos);

	float distance = glm::distance(m_Pos, listenerPos);

	return distance > defPtr->maxDistance;
}


Implementation::Implementation() : m_NextChannelid(0), m_NextSoundid(0)
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


void Implementation::Update(float deltaTime, glm::vec2 listenerPos)
{
	auto pos = FmodSoundSystem::VectorToFmod({ listenerPos.x, listenerPos.y });
	m_System->set3DListenerAttributes(0, &pos, {}, {}, {}); // TODO: change how we set listener pos

	std::vector<ChannelMap::iterator> stoppedChannels;
	for (auto it = m_Channels.begin(), itEnd = m_Channels.end(); it != itEnd; ++it)
	{
		it->second->Update(deltaTime);
		if (it->second->m_State == Channel::State::STOPPED)
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

void Implementation::LoadSound(idx soundId)
{
	// seems useless unless my implementation is wrong
	//if (IsSoundLoaded(soundId))
	//	return;

	auto foundIt = m_Sounds.find(soundId);
	if (foundIt == m_Sounds.end())
		return;

	FMOD_MODE mode = FMOD_NONBLOCKING;
	mode |= false ? (FMOD_3D | FMOD_3D_INVERSETAPEREDROLLOFF) : FMOD_2D; // Later add is3D?
	mode |= foundIt->second->m_SoundDefinition.isLooping ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
	mode |= foundIt->second->m_SoundDefinition.isStreaming ? FMOD_CREATESTREAM : FMOD_CREATECOMPRESSEDSAMPLE;


	FmodSoundSystem::ErrorCheck(m_System->createSound(foundIt->second->m_SoundDefinition.soundName.c_str(), mode, nullptr, 
														&foundIt->second->m_Sound));

	if (foundIt->second->m_Sound)
	{
		foundIt->second->m_Sound->set3DMinMaxDistance(foundIt->second->m_SoundDefinition.minDistance,
														foundIt->second->m_SoundDefinition.maxDistance);
	}
}

bool Implementation::IsSoundLoaded(idx soundId)
{
	auto foundIt = m_Sounds.find(soundId);
	if (foundIt == m_Sounds.end())
		return false;
	
	// Check if the sound is fully loaded
	FMOD_OPENSTATE openState;
	FmodSoundSystem::ErrorCheck(foundIt->second->m_Sound->getOpenState(&openState, nullptr, nullptr, nullptr));
	
	return(openState == FMOD_OPENSTATE_READY);
}

void FmodSoundSystem::Init()
{
	if(!m_SoundEngineImpl)
		m_SoundEngineImpl = new Implementation;
}

void FmodSoundSystem::Update(float deltaTime, glm::vec2 listenerPos)
{
	m_SoundEngineImpl->Update(deltaTime, listenerPos);
}

void FmodSoundSystem::Shutdown()
{
	delete m_SoundEngineImpl;
}

int FmodSoundSystem::ErrorCheck(FMOD_RESULT result [[maybe_unused]] )
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

idx FmodSoundSystem::RegisterSound(const SoundDefinition& soundDef, bool load)
{
	idx soundId = m_SoundEngineImpl->m_NextSoundid++;

	m_SoundEngineImpl->m_Sounds[soundId] = std::make_unique<Implementation::Sound>(soundDef);

	if (load)
		LoadSound(soundId);

	return soundId;
}


// TODO: possibly improve
void FmodSoundSystem::UnregisterSound(idx nSoundId)
{
	UnloadSound(nSoundId);
	//auto soundIt = m_SoundEngineImpl->m_Sounds.find(nSoundId);
	//if (soundIt == m_SoundEngineImpl->m_Sounds.end()) 
	//	return;
	//m_SoundEngineImpl->m_Sounds.erase(soundIt);
}

void FmodSoundSystem::LoadSound(idx soundId)
{
	m_SoundEngineImpl->LoadSound(soundId);
}

//bool FmodSoundSystem::IsSoundReady(const std::string& soundName)
//{
//	auto foundIt = m_SoundEngineImpl->m_Sounds.find(soundName);
//	if (foundIt == m_SoundEngineImpl->m_Sounds.end())
//		return false;
//
//	// Check if the sound is fully loaded
//	FMOD_OPENSTATE openState;
//	ErrorCheck(foundIt->second->getOpenState(&openState, nullptr, nullptr, nullptr));
//
//	return(openState == FMOD_OPENSTATE_READY);
//}

void FmodSoundSystem::UnloadSound(idx soundId)
{
	auto foundIt = m_SoundEngineImpl->m_Sounds.find(soundId);
	if (foundIt == m_SoundEngineImpl->m_Sounds.end())
		return;
	foundIt->second->m_Sound->release();
	m_SoundEngineImpl->m_Sounds.erase(foundIt);
}

void FmodSoundSystem::Set2dListenerAndOrientation(const glm::vec2&, const glm::vec2&)
{
	//TODO: FMOD listenener
}

//idx FmodSoundSystem::PlaySound(const std::string& soundName, const glm::vec2& pos, float volumedB)
//{
//	const idx nChannelId = m_SoundEngineImpl->m_NextChannelid++;
//    auto foundIt = m_SoundEngineImpl->m_Sounds.find(soundName);
//    if (foundIt == m_SoundEngineImpl->m_Sounds.end())
//    {
//        LoadSound(soundName);
//        foundIt = m_SoundEngineImpl->m_Sounds.find(soundName);
//        if (foundIt == m_SoundEngineImpl->m_Sounds.end())
//        {
//            return nChannelId;
//        }
//    }
//    FMOD::Channel* pChannel = nullptr;
//    ErrorCheck(m_SoundEngineImpl->m_System->playSound(foundIt->second, nullptr, true, &pChannel));
//    if(pChannel)
//    {
//        FMOD_VECTOR position = VectorToFmod(pos);
//        pChannel->set3DAttributes(&position, nullptr);
//        pChannel->setVolume(dBToVolume(volumedB));
//        pChannel->setPaused(false);
//        m_SoundEngineImpl->m_Channels[nChannelId] = pChannel;
//    }
//    return nChannelId;
//}

idx FmodSoundSystem::PlaySound(idx nSoundId, const glm::vec2& pos, float volumedB)
{
	const idx nChannelId = m_SoundEngineImpl->m_NextChannelid++;
	auto soundIt = m_SoundEngineImpl->m_Sounds.find(nSoundId);
	if (soundIt == m_SoundEngineImpl->m_Sounds.end())
		return nChannelId;

	m_SoundEngineImpl->m_Channels[nChannelId] =
		std::make_unique<Implementation::Channel>(*m_SoundEngineImpl, nSoundId,
			soundIt->second->m_SoundDefinition, pos, volumedB); //check if this is correct

	return nChannelId;
}

void FmodSoundSystem::StopChannel(idx nChannelId, float fadeTimeSeconds)
{
	auto foundIt = m_SoundEngineImpl->m_Channels.find(nChannelId);
	if (foundIt == m_SoundEngineImpl->m_Channels.end())
		return;

	if (fadeTimeSeconds <= 0)
	{
		foundIt->second->m_Channel->stop();
	}
	else
	{
		foundIt->second->m_StopRequested = true;
		foundIt->second->m_StopFader->StartFade(SILENCE_dB, fadeTimeSeconds); //TODO
	}
}

void FmodSoundSystem::StopAllChannels()
{
	//also works as (see below) or just use .second 
	//for (const auto& channel : m_SoundEngineImpl->m_Channels | std::views::values)
	for (const auto& channel : std::views::values(m_SoundEngineImpl->m_Channels))
	{
		channel->m_Channel->stop();
	}
}

void FmodSoundSystem::SetChannel2dPosition(idx nChannelId, const glm::vec2& pos)
{
	auto foundIt = m_SoundEngineImpl->m_Channels.find(nChannelId);
	if (foundIt == m_SoundEngineImpl->m_Channels.end())
		return;

	foundIt->second->m_Pos = pos;
}

void FmodSoundSystem::SetChannelVolume(idx nChannelId, float volumedB)
{
	auto foundIt = m_SoundEngineImpl->m_Channels.find(nChannelId);
	if (foundIt == m_SoundEngineImpl->m_Channels.end())
		return;

	foundIt->second->m_VolumedB = volumedB;
}

bool FmodSoundSystem::IsPlaying(idx nChannelId) const
{
	auto foundIt = m_SoundEngineImpl->m_Channels.find(nChannelId);
	if (foundIt == m_SoundEngineImpl->m_Channels.end())
		return false;

	bool retVal;
	foundIt->second->m_Channel->isPlaying(&retVal);
	return retVal;
}

//void FmodSoundSystem::LoadBank(const std::string& strBankName, FMOD_STUDIO_LOAD_BANK_FLAGS flags)
//{
//	auto tFoundIt = m_SoundEngineImpl->m_Banks.find(strBankName);
//	if (tFoundIt != m_SoundEngineImpl->m_Banks.end())
//		return;
//	FMOD::Studio::Bank* pBank;
//	ErrorCheck(m_SoundEngineImpl->m_StudioSystem->loadBankFile(strBankName.c_str(), flags, &pBank));
//	if (pBank)
//	{
//		m_SoundEngineImpl->m_Banks[strBankName] = pBank;
//	}
//}
//
//void FmodSoundSystem::LoadEvent(const std::string& eventName)
//{
//	auto foundit = m_SoundEngineImpl->m_Events.find(eventName);
//	if (foundit != m_SoundEngineImpl->m_Events.end())
//		return;
//	FMOD::Studio::EventDescription* pEventDescription = nullptr;
//	ErrorCheck(m_SoundEngineImpl->m_StudioSystem->getEvent(eventName.c_str(), &pEventDescription));
//	if (pEventDescription) {
//		FMOD::Studio::EventInstance* pEventInstance = nullptr;
//		ErrorCheck(pEventDescription->createInstance(&pEventInstance));
//		if (pEventInstance) {
//			m_SoundEngineImpl->m_Events[eventName] = pEventInstance;
//		}
//	}
//}
//
//void FmodSoundSystem::PlayEvent(const std::string& eventName)
//{
//	auto foundit = m_SoundEngineImpl->m_Events.find(eventName);
//	if (foundit == m_SoundEngineImpl->m_Events.end()) {
//		LoadEvent(eventName);
//		foundit = m_SoundEngineImpl->m_Events.find(eventName);
//		if (foundit == m_SoundEngineImpl->m_Events.end())
//			return;
//	}
//	foundit->second->start();
//}
//
//void FmodSoundSystem::StopEvent(const std::string& eventName, bool immediate)
//{
//	auto foundIt = m_SoundEngineImpl->m_Events.find(eventName);
//	if (foundIt == m_SoundEngineImpl->m_Events.end())
//		return;
//	FMOD_STUDIO_STOP_MODE eMode;
//	eMode = immediate ? FMOD_STUDIO_STOP_IMMEDIATE : FMOD_STUDIO_STOP_ALLOWFADEOUT;
//	ErrorCheck(foundIt->second->stop(eMode));
//}
//
//void FmodSoundSystem::GetEventParameter(const std::string& eventName, const std::string& parameterName, float* parameter)
//{
//	auto foundIt = m_SoundEngineImpl->m_Events.find(eventName);
//	if (foundIt == m_SoundEngineImpl->m_Events.end())
//		return;
//
//	ErrorCheck(foundIt->second->getParameterByName(parameterName.c_str(), parameter));
//}
//
//void FmodSoundSystem::SetEventParameter(const std::string& eventName, const std::string& parameterName, float value)
//{
//	auto foundIt = m_SoundEngineImpl->m_Events.find(eventName);
//	if (foundIt == m_SoundEngineImpl->m_Events.end())
//		return;
//
//	ErrorCheck(foundIt->second->setParameterByName(parameterName.c_str(), value));
//}
//
//bool FmodSoundSystem::IsEventPlaying(const std::string& eventName) const
//{
//	auto foundIt = m_SoundEngineImpl->m_Events.find(eventName);
//	if (foundIt == m_SoundEngineImpl->m_Events.end())
//		return false;
//
//	FMOD_STUDIO_PLAYBACK_STATE* state = nullptr;
//	if (static_cast<FMOD_STUDIO_PLAYBACK_STATE>(foundIt->second->getPlaybackState(state)) == FMOD_STUDIO_PLAYBACK_PLAYING) {
//		return true;
//	}
//	return false;
//}
