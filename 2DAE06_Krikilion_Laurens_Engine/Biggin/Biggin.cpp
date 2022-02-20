#include "BigginPCH.h"
#include "Biggin.h"
#include <thread>

#include "FpsCounter.h"
#include "InputManager.h"
#include "SceneManager.h"
#include "Renderer.h"
#include "ResourceManager.h"
#include "TextObject.h"
#include "GameObject.h"
#include "GameTime.h"
#include "Logger.h"
#include "Scene.h"
#include "GameObject.h"
#include "Sprite.h"
#include "Texture2D.h"
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
		640,
		480,
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
	Logger::GetInstance().LogInfo("Created Demo scene");

	auto TextureGO = std::make_shared<GameObject>();

	TextureGO->AddComponent(std::make_shared<Sprite>("background.jpg"));
	scene.Add(TextureGO);
	Logger::GetInstance().LogInfo("Added background to scene");

	TextureGO = std::make_shared<GameObject>();



	TextureGO->AddComponent(std::make_shared<Sprite>("logo.png"));
	TextureGO->SetPosition(216, 180);
	scene.Add(TextureGO);
	Logger::GetInstance().LogInfo("Added logo to scene");

	TextureGO = std::make_shared<GameObject>();



	auto font = ResourceManager::GetInstance().LoadFont("Lingua.otf", 36);
	auto titleTo = std::make_shared<TextObject>("Programming 4 Assignment", font);
	titleTo->SetPosition(80, 20);
	TextureGO->AddComponent(titleTo);
	scene.Add(TextureGO);
	Logger::GetInstance().LogInfo("Added Title to scene");

	TextureGO = std::make_shared<GameObject>();



	auto fpsTo = std::make_shared<FpsCounter>(scene);
	TextureGO->AddComponent(fpsTo);
	scene.Add(TextureGO);
	Logger::GetInstance().LogInfo("Added FpsCounter to scene");
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

		// todo: this update loop could use some work.
		//bool doContinue = true;
		//while (doContinue)
		//{
		//	doContinue = input.ProcessInput();
		//	sceneManager.Update();
		//	renderer.Render();
		//}


		bool doContinue = true;
		auto lastTime = chrono::high_resolution_clock::now();
		float lag = 0.0f;
		while (doContinue)
		{
			
			const auto currentTime = chrono::high_resolution_clock::now();
			float deltaTime = chrono::duration<float>(currentTime - lastTime).count();
			lastTime = currentTime;
			lag += deltaTime;
			doContinue = input.ProcessInput();
			sceneManager.Update();
			while (lag >= MsPerFrame)
			{
				sceneManager.FixedUpdate(m_FixedTimeStep);
				lag -= m_FixedTimeStep;
			}
			renderer.Render();
		}
	}

	Cleanup();
}
