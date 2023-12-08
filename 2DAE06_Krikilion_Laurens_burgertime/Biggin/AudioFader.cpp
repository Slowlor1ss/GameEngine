#include "BigginPCH.h"
//#include "AudioFader.h"
//#include "FModSoundSystem.h"
//#include <algorithm>

//AudioFader::AudioFader(Implementation::Channel* channel) : m_ChannelRef(channel)
//{
//}
//
//void AudioFader::StartFade(float toVolumedB, float fadeTimeSeconds)
//{
//	FromVolumedB = GetCurrentVolume();
//	ToVolumedB = toVolumedB;
//	CurrentTime = 0.0f;
//	TimeFade = fadeTimeSeconds;
//}
//
//void AudioFader::StartFade(float fromVolumedB, float toVolumedB, float fadeTimeSeconds)
//{
//	FromVolumedB = fromVolumedB;
//	ToVolumedB = toVolumedB;
//	CurrentTime = 0.0f;
//	TimeFade = fadeTimeSeconds;
//}
//
//void AudioFader::Update(float deltaTime)
//{
//	m_ChannelRef->m_VolumedB = GetCurrentVolumedB();
//	CurrentTime += deltaTime;
//}
//
//bool AudioFader::IsFinished() const
//{
//	return CurrentTime >= TimeFade;
//}
//
//float AudioFader::GetCurrentVolumedB() const {
//	return std::lerp(FromVolumedB, ToVolumedB, std::clamp(CurrentTime / TimeFade, 0.0f, 1.0f));
//}
//
//float AudioFader::GetCurrentVolume() const {
//	return FmodSoundSystem::dBToVolume(GetCurrentVolumedB());
//}
