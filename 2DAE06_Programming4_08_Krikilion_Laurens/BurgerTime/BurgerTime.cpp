//#include "BigginPCH.h"

#if _DEBUG
#if __has_include(<vld.h>)
#include <vld.h>
#endif
#endif

#include <Biggin.h>
#include <iostream>

#pragma warning(push, 0)
#include <steam_api.h>
#include <Box2D/Box2D.h>
#pragma warning(pop)
#include "SimpleSDL2SoundSystem.h"
#include "audio.h"
#include "GameLoader.h"
#include "GameObject.h"
#include "MenuRenderer.h"
#include "Scene.h"
#include "SceneManager.h"

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

	biggin::SceneManager::GetInstance().CreateScene("MainMenu");

	biggin::Biggin engine;
	engine.Initialize();

	std::cout << "==============================================================================\r\n     __________  ____  ____ _________    ________  __________ _________\r\n     |   ___   \\ |  |  |  | |   ___  \\  /  ______| | _______| |   ___  \\\r\n     |   |_|    ||  |  |  | |   |_|   ||  /        | |        |   |_|   |\r\n     |         / |  |  |  | | ___  __/ | |   ____  | |__      | ___  __/\r\n     |   ___   \\ |  \\__/  | | |  \\ \\   | |  |___ | | ___|     | |  \\ \\\r\n     |   |_|    ||        | | |   \\ \\  | |_____| | | |_______ | |   \\ \\\r\n     |_________/  \\______/  |_|    \\_\\  \\_______/  |________| |_|    \\_\\ \r\n\r\n==============================================================================\r\n                __________ __________ ___    ___ __________\r\n                |___  ___| |___  ___| |  \\  /  | | _______|\r\n                   |  |       |  |    |   \\/   | | |\r\n                   |  |       |  |    |        | | |__\r\n                   |  |       |  |    |  \\  /  | | ___|\r\n                   |  |    ___|  |___ |  |\\/|  | | |_______\r\n                   |__|    |________| |__|  |__| |________|\r\n\r\n==============================================================================\r\n\t\t\t+=+=+=+=+=+=+=+=+=+=+=+=+=+=+\r\n==============================================================================\n";
	std::cout << "\nPress [c] for controls\n";
	std::cout << "\nPeter Piper picked a peck of pickled peppers.\n"
		<< "Did Peter Piper pick a peck of pickled peppers?\n"
		<< "If Peter Piper picked a peck of pickled peppers,\n"
		<< "where's the peck of pickled peppers Peter Piper picked? \n\n";

	biggin::MenuRenderer::GetInstance().SetMenu(new GameLoader());
	engine.Run();

	SteamAPI_Shutdown();

	return 0;
}