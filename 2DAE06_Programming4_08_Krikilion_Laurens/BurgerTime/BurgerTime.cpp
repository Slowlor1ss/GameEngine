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

	/*auto& scene = */biggin::SceneManager::GetInstance().CreateScene("MainMenu");
	//auto go = std::make_shared<biggin::GameObject>();
	//go->AddComponent(new GameLoader(go.get()));
	//scene.Add(go);
	biggin::MenuRenderer::GetInstance().SetMenu(new GameLoader());

	biggin::Biggin engine;
	std::cout << "click play to hear the sound\n";
	engine.Run();

	SteamAPI_Shutdown();

	return 0;
}