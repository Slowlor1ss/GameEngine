#include "BigginPCH.h"

#if _DEBUG
#if __has_include(<vld.h>)
#include <vld.h>
#endif
#endif

#include "Biggin.h"
#include <steam_api.h>
#include <Box2D/Box2D.h>
#include "LogSoundSystem.h"
#include "SimpleSDL2SoundSystem.h"
#include "SoundServiceLocator.h"
#include "audio.h"

#define ADD_CONSOLE_IN_RELEASE

int main(int /*argc*/, char* /*argv*/[])
{
	if (!SteamAPI_Init())
	{
		std::cerr << "Fatal Error - Steam must be running to play this game (SteamAPI_Init() failed)." << std::endl;
		return 1;
	}
	else
		std::cout << "Successfully initialized steam." << std::endl;


#if defined(_WIN32) && SDL_VERSION_ATLEAST(2, 0, 8)
	if (!getenv("SDL_AUDIODRIVER")) {
		_putenv_s("SDL_AUDIODRIVER", "directsound");
	}
#endif

	/* Initialize only SDL Audio on default device */
	if (SDL_Init(SDL_INIT_AUDIO) < 0)
	{
		std::cerr <<  "Fatal Error - Failed to init SDL audio";
		return 1;
	}

	// Initialize Simple-SDL2-Audio
	initAudio();

#if _DEBUG
	SoundServiceLocator::RegisterSoundSystem(new LogSoundSystem(new SimpleSDL2SoundSystem("../Data/Sounds/")));
#else
	SoundServiceLocator::RegisterSoundSystem(new SimpleSDL2SoundSystem("../Data/Sounds/"));
#endif

	//TODO: just for testing will move this to a correct location later
	SoundServiceLocator::GetSoundSystem().Play("main.wav", 0.2f, false);

	biggin::Biggin engine;
	engine.Run();

	SteamAPI_Shutdown();

	return 0;
}