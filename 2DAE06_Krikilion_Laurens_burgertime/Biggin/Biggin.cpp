#include "BigginPCH.h"
#include "Biggin.h"
#include <SDL_image.h>
#include <steam_api.h>
#include <steam_api_common.h>
#include <thread>
#include "audio.h"
#include "Box2dManager.h"
#include "BoxCollider2d.h"
#include "Command.h"
#include "FpsCounter.h"
#include "InputManager.h"
#include "SceneManager.h"
#include "Renderer.h"
#include "ResourceManager.h"
#include "TextComponent.h"
#include "GameObject.h"
#include "GameTime.h"
#include "Logger.h"
#include "Scene.h"
#include "LogSoundSystem.h"
#include "RenderComponent.h"
#include "SimpleSDL2SoundSystem.h"
#include "SoundServiceLocator.h"
#include "SpriteRenderComponent.h"
#include "Subject.h"

using namespace std;

void PrintSDLVersion()
{
	SDL_version compiled{};
	SDL_version linked{};

	SDL_VERSION(&compiled);
	SDL_GetVersion(&linked);
	printf("We compiled against SDL version %d.%d.%d ...\n",
		compiled.major, compiled.minor, compiled.patch);
	printf("We are linking against SDL version %d.%d.%d.\n",
		linked.major, linked.minor, linked.patch);
}

void biggin::Biggin::Initialize()
{
	//seeding rand
	srand(static_cast<unsigned>(time(nullptr)));

	PrintSDLVersion();
	
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
		throw std::runtime_error(std::string("SDL_Init Error: ") + SDL_GetError());

#if defined(_WIN32) && SDL_VERSION_ATLEAST(2, 0, 8)
	if (!getenv("SDL_AUDIODRIVER")) {
		_putenv_s("SDL_AUDIODRIVER", "directsound");
	}
#endif

	/* Initialize only SDL Audio on default device */
	if (SDL_Init(SDL_INIT_AUDIO) < 0)
		throw std::runtime_error(std::string("SDL_Init Error: ") + SDL_GetError());

	// Initialize Simple-SDL2-Audio
	initAudio();

#if _DEBUG
	SoundServiceLocator::RegisterSoundSystem(new LogSoundSystem(new SimpleSDL2SoundSystem("../Data/Sounds/")));
#else
	SoundServiceLocator::RegisterSoundSystem(new SimpleSDL2SoundSystem("../Data/Sounds/"));
#endif

	m_Window = SDL_CreateWindow(
		"BurgerTime",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		m_WindowWidth, //640
		m_WindowHeight, //480
		SDL_WINDOW_OPENGL
	);
	if (m_Window == nullptr)
		throw std::runtime_error(std::string("SDL_CreateWindow Error: ") + SDL_GetError());

	SDL_Surface* icon = IMG_Load("../Data/icon.png");
	if (icon != nullptr)
		SDL_SetWindowIcon(m_Window, icon);

	Renderer::GetInstance().Init(m_Window);
	Logger::GetInstance().Initialize();
	// tell the resource manager where he can find the game data
	ResourceManager::GetInstance().Init("../Data/");
	//Box2dManager::GetInstance().Init();

	SteamAPI_RunCallbacks();
}

void biggin::Biggin::Cleanup()
{
	Renderer::GetInstance().Destroy();
	SDL_DestroyWindow(m_Window);
	m_Window = nullptr;
	SoundServiceLocator::Cleanup();
	SDL_Quit();
}

void biggin::Biggin::Run()
{

	//LoadGame();

	{
		const auto& renderer = Renderer::GetInstance();
		const auto& sceneManager = SceneManager::GetInstance();
		const auto& input = InputManager::GetInstance();
		auto& gameTime = GameTime::GetInstance();
		//const auto& box2dManager = Box2dManager::GetInstance();

		bool doContinue = true;
		
		float lag = 0.0f;
		while (doContinue)
		{
			SteamAPI_RunCallbacks();

			//updates things like delta-time fps counter etc
			//(maybe a bit of a questionable way of doing things but i found it clean to have all of this is one class)
			gameTime.Update();
			const float deltaTime = gameTime.GetDeltaT();

			lag += deltaTime;
			doContinue = input.ProcessInput();
			sceneManager.Update();

			while (lag >= gameTime.GetFixedTimeStep())
			{
				//box2dManager.Simulate();

				sceneManager.FixedUpdate();
				lag -= gameTime.GetFixedTimeStep();
			}
			renderer.Render();
		}
	}

	Cleanup();
}

