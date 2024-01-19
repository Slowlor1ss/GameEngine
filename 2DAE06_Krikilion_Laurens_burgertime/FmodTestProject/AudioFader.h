#pragma once
#include "FModSoundSystem.h"

class AudioFader
{
public:
	AudioFader(Implementation::Channel* channel);

	// Fade in
	void StartFade(float toVolumedB, float fadeTimeSeconds);
	// Fade out
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

