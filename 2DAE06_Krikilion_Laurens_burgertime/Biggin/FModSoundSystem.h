#pragma once

#include <map>
#include <fmod_studio.hpp>
#include <fmod.hpp>
#include <memory>
//#include "AudioFader.h"

struct Implementation;
class AudioFader;

#define VIRTUALIZE_FADE_TIME 1.0f
#define SILENCE_dB -120.f
// TODO: Add audio namespace
using idx = int32_t;

class FmodSoundSystem
{

	inline static Implementation* m_SoundEngineImpl{ nullptr };

public:
	static void Init();
	static void Update(float deltaTime, glm::vec2 listenerPos);
	static void Shutdown();
	static int	ErrorCheck(FMOD_RESULT result);

	struct SoundDefinition
	{
		std::string soundName;
		float		defaultVolumedB;
		float		minDistance;
		float		maxDistance;
		bool		isLooping;
		bool		isStreaming;
	};
	idx		RegisterSound(const SoundDefinition& soundDef, bool load = true);
	void	UnregisterSound(idx nSoundId);
	void	LoadSound(idx soundId);
	//bool	IsSoundLoaded(idx soundId);
	//bool	IsSoundReady(const std::string& soundName);
	void	UnloadSound(idx soundId);
	void	Set2dListenerAndOrientation(const glm::vec2& pos, const glm::vec2& look);
	int		PlaySound(idx nSoundId, const glm::vec2& pos = glm::vec2{ 0, 0 }, float volumedB = 0.0f);
	void	StopChannel(idx nChannelId, float fadeTimeSeconds);
	void	StopAllChannels();
	void	SetChannel2dPosition(idx nChannelId, const glm::vec2& pos);
	void	SetChannelVolume(idx nChannelId, float volumedB);
	bool	IsPlaying(idx nChannelId) const;

	// TODO: Update Banks and Events to use indexes instead of strings
	//void	LoadBank(const std::string& strBankName, FMOD_STUDIO_LOAD_BANK_FLAGS flags);
	//void	LoadEvent(const std::string& eventName);
	//void	PlayEvent(const std::string& eventName);
	//void	StopEvent(const std::string& eventName, bool immediate = false);
	//void	GetEventParameter(const std::string& eventName, const std::string& eventParameter, float* parameter);
	//void	SetEventParameter(const std::string& eventName, const std::string& parameterName, float value);
	//bool	IsEventPlaying(const std::string& eventName) const;

	// Utils

	static constexpr FMOD_VECTOR VectorToFmod(const glm::vec2& pos)
	{
		FMOD_VECTOR fVec{};
		fVec.x = pos.x;
		fVec.y = pos.y;
		fVec.z = 0;
		return fVec;
	}

	static glm::vec2 FmodToVector(const FMOD_VECTOR& pos)
	{
		glm::vec2 vec{};
		vec.x = pos.x;
		vec.y = pos.y;
		return vec;
	}

	// 40dB is 100 gain
	static float dBToVolume(float dB)
	{
		return powf(10.0f, 0.05f * dB);
	}

	static float VolumeTodB(float volume)
	{
		return 20.0f * log10f(volume);
	}
};

struct Implementation
{

	struct Sound
	{
		Sound(const FmodSoundSystem::SoundDefinition& soundDefinition, bool oneShot = false);

		FMOD::Sound* m_Sound{ nullptr };
		FmodSoundSystem::SoundDefinition m_SoundDefinition{};
		bool m_OneShot = false;
	};

	struct Channel
	{
		Channel(Implementation& implementation, idx soundId, const FmodSoundSystem::SoundDefinition& soundDefinition,
			const glm::vec2& pos, float volumedB);
		~Channel();

		enum class State
		{
			INITIALIZE, TOPLAY, LOADING, PLAYING, STOPPING, STOPPED, VIRTUALIZING, VIRTUAL, DEVIRTUALIZE,
		};

		Implementation& m_EngineImpl;
		FMOD::Channel* m_Channel;
		idx m_SoundId;
		glm::vec2 m_Pos;
		float m_VolumedB;
		const float m_OrigVolumedB;
		State m_State = State::INITIALIZE;
		bool m_StopRequested;

		AudioFader* m_StopFader;
		AudioFader* m_VirtualizeFader;

		void Update(float deltaTime);
		void UpdateChannelParameters();
		bool ShouldBeVirtual(bool allowVirtualOneShot) const;
		bool IsPlaying() const;
		float GetVolumedB() const;
	private:
		bool IsOneShot() const;
		const FmodSoundSystem::SoundDefinition* GetSoundDefinition() const;
	};


	Implementation();
	~Implementation();

	void Update(float deltaTime, glm::vec2 listenerPos);

	void LoadSound(idx soundId);
	bool IsSoundLoaded(idx soundId);

	FMOD::Studio::System* m_StudioSystem;
	FMOD::System* m_System;

	idx m_NextSoundid;
	idx m_NextChannelid;

	//TODO: Benchmark to see if std::unordered_map is not faster for the usage.
	typedef std::map<idx, std::unique_ptr<Sound>> SoundMap;
	typedef std::map<idx, std::unique_ptr<Channel>> ChannelMap;
	typedef std::map<std::string, FMOD::Studio::EventInstance*> EventMap;
	typedef std::map<std::string, FMOD::Studio::Bank*> BankMap;
	BankMap m_Banks;
	EventMap m_Events;
	SoundMap m_Sounds;
	ChannelMap m_Channels;
};

class AudioFader
{
public:
	AudioFader(Implementation::Channel* channel);

	void StartFade(float toVolumedB, float fadeTimeSeconds);
	void StartFade(float fromVolumedB, float toVolumedB, float fadeTimeSeconds);
	// Update fade
	void Update(float deltaTime);
	bool IsFinished() const;
	float GetCurrentVolumedB() const;
	float GetCurrentVolume() const;
private:
	Implementation::Channel* m_ChannelRef;
	float FromVolumedB = 0.0f;
	float ToVolumedB = 0.0f;
	float TimeFade = 0.0f;
	float CurrentTime = 0.0f;
};
