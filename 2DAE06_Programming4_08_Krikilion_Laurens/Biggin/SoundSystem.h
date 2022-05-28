#pragma once
class SoundSystem
{
public:
	virtual ~SoundSystem() = default;

	virtual void Play(const std::string& filename, float volume, bool isSoundEffect = true) = 0;
	virtual void StopAll() = 0;
};
