#pragma once

#include <map>
#include <fmod_studio.hpp>
#include <fmod.hpp>

class AudioFader
{
private:
	float FromVolumedB = 0.0f;
	float ToVolumedB = 0.0f;
	float TimeFade = 0.0f;
	float CurrentTime = 0.0f;
public:
	// Fade in
	void StartFade(float toVolumedB, float fadeTimeSeconds);
	// Fade out
	void StartFade(float fromVolumedB, float toVolumedB, float fadeTimeSeconds);
	// Update fade
	void Update(float deltaTime);
	bool IsFinished() const;
	float GetCurrentVolumedB() const;
	float GetCurrentVolume() const;
};

struct Sound
{
	Sound(const FmodSoundSystem::SoundDefinition&, bool oneShot = false);

	FMOD::Sound* m_sound{ nullptr };
	FmodSoundSystem::SoundDefinition m_SoundDefinition{};
	bool m_OneShot = false;
};

struct Channel
{
	Channel(Implementation& implementation, int soundId, const FmodSoundSystem::SoundDefinition& soundDefinition,
		const glm::vec2& pos, float volumedB);

	enum class State
	{ INITIALIZE, TOPLAY, LOADING, PLAYING, STOPPING, STOPPED, VIRTUALIZING, VIRTUAL, DEVIRTUALIZE, };

	FmodSoundSystem& m_Engine;
	FMOD::Channel* m_Channel;
	int m_SoundId;
	glm::vec2 m_Pos;
	float m_VolumedB;
	float m_SoundVolume;
	State m_State = State::INITIALIZE;
	bool m_StopRequested;

	AudioFader m_StopFader;
	AudioFader m_VirtualizeFader;

	void Update(float deltaTime);
	void UpdateChannelParameters();
	bool ShouldBeVirtual(bool allowVirtualOneShot) const;
	bool IsPlaying() const;
	float GetVolumedB() const;
private:
	bool IsOneShot() const;
	const FmodSoundSystem::SoundDefinition* GetSoundDefinition() const;
};

struct Implementation
{
	Implementation();
	~Implementation();

	void Update();

	FMOD::Studio::System* m_StudioSystem;
	FMOD::System* m_System;

	int m_NextChannelid;

	typedef std::map<int, Sound*> SoundMap;
	typedef std::map<int, Channel*> ChannelMap;
	typedef std::map<std::string, FMOD::Studio::EventInstance*> EventMap;
	typedef std::map<std::string, FMOD::Studio::Bank*> BankMap;
	BankMap m_Banks;
	EventMap m_Events;
	SoundMap m_Sounds;
	ChannelMap m_Channels;

};

class FmodSoundSystem
{
	inline static Implementation* m_SoundEngineImpl{ nullptr };

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

	void	LoadSound(int soundId);
	bool	IsSoundLoaded(int soundId);
	bool	IsSoundReady(const std::string& soundName);
	void	UnloadSound(const std::string& soundName);
	void	Set2dListenerAndOrientation(const glm::vec2& pos, const glm::vec2& look);
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
		FMOD_VECTOR fVec{};
		fVec.x = pos.x;
		fVec.y = pos.y;
		fVec.z = 0;
		return fVec;
	}

	static float dBToVolume(float dB)
	{
		return powf(10.0f, 0.05f * dB);
	}

	static float VolumeTodB(float volume)
	{
		return 20.0f * log10f(volume);
	}
};

