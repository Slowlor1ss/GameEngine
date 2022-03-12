#include "BigginPCH.h"
#include "Biggin.h"
#include <thread>

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
#include "GameObject.h"
#include "Sprite.h"
#include "Texture2D.h"
#include "TrashTheCache.h"
#include "Utils.hpp"

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

void dae::Biggin::Initialize()
{
	PrintSDLVersion();
	
	if (SDL_Init(SDL_INIT_VIDEO) != 0) 
	{
		throw std::runtime_error(std::string("SDL_Init Error: ") + SDL_GetError());
	}

	m_Window = SDL_CreateWindow(
		"Programming 4 assignment",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		1100, //640
		600, //480
		SDL_WINDOW_OPENGL
	);
	if (m_Window == nullptr) 
	{
		throw std::runtime_error(std::string("SDL_CreateWindow Error: ") + SDL_GetError());
	}

	Renderer::GetInstance().Init(m_Window);

	Logger::GetInstance().Initialize();
}

/**
 * Code constructing the scene world starts here
 */
void dae::Biggin::LoadGame() const
{
	auto& scene = SceneManager::GetInstance().CreateScene("Demo");

	auto Go = std::make_shared<GameObject>();

	Go->AddComponent(std::make_shared<RenderComponent>(Go.get(), "background.jpg"));
	scene.Add(Go);



	Go = std::make_shared<GameObject>();

	Go->AddComponent(std::make_shared<RenderComponent>(Go.get(), "logo.png"));
	Go->SetPosition(216, 180);
	scene.Add(Go);



	Go = std::make_shared<GameObject>();

	{
		Go->AddComponent(std::make_shared<RenderComponent>(Go.get()));
		scene.Add(Go);

		auto font = ResourceManager::GetInstance().LoadFont("Lingua.otf", 36);
		auto titleText = std::make_shared<TextComponent>(Go.get(), "Programming 4 Assignment", font);
		Go->SetPosition(80, 20);
		Go->AddComponent(titleText);
		scene.Add(Go);
	}



	Go = std::make_shared<GameObject>();

	Go->AddComponent(std::make_shared<RenderComponent>(Go.get()));
	scene.Add(Go);

	auto fpsText = std::make_shared<TextComponent>(Go.get());
	Go->SetPosition(10, 10);
	fpsText->SetColor({ 0, 255, 0, 1 });
	Go->AddComponent(fpsText);
	scene.Add(Go);

	auto fpsTo = std::make_shared<FpsCounter>(Go.get());
	Go->AddComponent(fpsTo);
	scene.Add(Go);

	//Add trash the cache
	Go = std::make_shared<GameObject>();

	Go->AddComponent(std::make_shared<TrashTheCache>(Go.get()));
	scene.Add(Go);

}

void dae::Biggin::Cleanup()
{
	Renderer::GetInstance().Destroy();
	SDL_DestroyWindow(m_Window);
	m_Window = nullptr;
	SDL_Quit();
}

void dae::Biggin::Run()
{
	Initialize();

	// tell the resource manager where he can find the game data
	ResourceManager::GetInstance().Init("../Data/");

	LoadGame();

	{
		auto& renderer = Renderer::GetInstance();
		auto& sceneManager = SceneManager::GetInstance();
		auto& input = InputManager::GetInstance();
		auto& gameTime = GameTime::GetInstance();

		bool doContinue = true;
		
		float lag = 0.0f;
		while (doContinue)
		{
			//updates things like delta-time fps counter etc
			//(maybe a bit of a questionable way of doing things but i found it clean to have all of this is one class)
			gameTime.Update();
			const float deltaTime = gameTime.GetDeltaT();

			lag += deltaTime;
			doContinue = input.ProcessInput();
			sceneManager.Update();

			while (lag >= gameTime.GetFixedTimeStep())
			{
				sceneManager.FixedUpdate();
				lag -= gameTime.GetFixedTimeStep();
			}
			renderer.Render();
		}
	}

	Cleanup();
}
