#pragma once
#include "SoundSystem.h"
#include "fmod_studio.hpp"
#include "fmod.hpp"
#include <map>

struct Implementation
{
	Implementation();
	~Implementation();

	void Update();

	FMOD::Studio::System* m_StudioSystem;
	FMOD::System* m_System;

	int m_NextChannelid;

	typedef std::map<std::string, FMOD::Sound*> SoundMap;
	typedef std::map<int, FMOD::Channel*> ChannelMap;
	typedef std::map<std::string, FMOD::Studio::EventInstance*> EventMap;
	typedef std::map<std::string, FMOD::Studio::Bank*> BankMap;
	BankMap m_Banks;
	EventMap m_Events;
	SoundMap m_Sounds;
	ChannelMap m_Channels;

};

class FmodSoundSystem
{
	static Implementation* m_SoundEngineImpl;

public:
	static void Init();
	static void Update();
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

	void	LoadSound(const std::string& soundName, bool isLooping = false, bool isStreaming = false);
	void	UnloadSound(const std::string& soundName);
	void	Set2dListenerAndOrientation(const glm::vec2& pos, const glm::vec2& vLook);
	int		PlaySound(const std::string& soundName, const glm::vec2& pos = glm::vec2{ 0, 0 }, float volumedB = 0.0f);
	void	StopChannel(int nChannelId);
	void	StopAllChannels();
	void	SetChannel2dPosition(int nChannelId, const glm::vec2& pos);
	void	SetChannelVolume(int nChannelId, float volumedB);
	bool	IsPlaying(int nChannelId) const;
	void	LoadBank(const std::string& strBankName, FMOD_STUDIO_LOAD_BANK_FLAGS flags);
	void	LoadEvent(const std::string& eventName);
	void	PlayEvent(const std::string& eventName);
	void	StopEvent(const std::string& eventName, bool immediate = false);
	void	GetEventParameter(const std::string& eventName, const std::string& eventParameter, float* parameter);
	void	SetEventParameter(const std::string& eventName, const std::string& parameterName, float value);
	bool	IsEventPlaying(const std::string& eventName) const;

	static constexpr FMOD_VECTOR VectorToFmod(const glm::vec2& pos)
	{
		FMOD_VECTOR fVec;
		fVec.x = pos.x;
		fVec.y = pos.y;
		fVec.z = 0;
		return fVec;
	}

	static float dbToVolume(float dB)
	{
		return powf(10.0f, 0.05f * dB);
	}

	static float VolumeTodB(float volume)
	{
		return 20.0f * log10f(volume);
	}
};

