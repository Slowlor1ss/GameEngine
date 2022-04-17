#include "BigginPCH.h"
#include "Biggin.h"
#include <steam_api_common.h>
#include <thread>
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
#include "HealthComponent.h"
#include "HealthUI.h"
#include "PeterPepper.h"
#include "RenderComponent.h"
#include "ScoreComponent.h"
#include "ScoreUI.h"
#include "SpriteRenderComponent.h"
#include "StatsAndAchievements.h"
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

	m_Window = SDL_CreateWindow(
		"BurgerTime",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		925, //640
		700, //480
		SDL_WINDOW_OPENGL
	);
	if (m_Window == nullptr)
		throw std::runtime_error(std::string("SDL_CreateWindow Error: ") + SDL_GetError());

	Renderer::GetInstance().Init(m_Window);

	Logger::GetInstance().Initialize();

	SteamAPI_RunCallbacks();
}

/**
 * Code constructing the scene world starts here
 */
void biggin::Biggin::LoadGame() const
{
	std::cout << "==============================================================================\r\n     __________  ____  ____ _________    ________  __________ _________\r\n     |   ___   \\ |  |  |  | |   ___  \\  /  ______| | _______| |   ___  \\\r\n     |   |_|    ||  |  |  | |   |_|   ||  /        | |        |   |_|   |\r\n     |         / |  |  |  | | ___  __/ | |   ____  | |__      | ___  __/\r\n     |   ___   \\ |  \\__/  | | |  \\ \\   | |  |___ | | ___|     | |  \\ \\\r\n     |   |_|    ||        | | |   \\ \\  | |_____| | | |_______ | |   \\ \\\r\n     |_________/  \\______/  |_|    \\_\\  \\_______/  |________| |_|    \\_\\ \r\n\r\n==============================================================================\r\n                __________ __________ ___    ___ __________\r\n                |___  ___| |___  ___| |  \\  /  | | _______|\r\n                   |  |       |  |    |   \\/   | | |\r\n                   |  |       |  |    |        | | |__\r\n                   |  |       |  |    |  \\  /  | | ___|\r\n                   |  |    ___|  |___ |  |\\/|  | | |_______\r\n                   |__|    |________| |__|  |__| |________|\r\n\r\n==============================================================================\r\n\t\t\t+=+=+=+=+=+=+=+=+=+=+=+=+=+=+\r\n==============================================================================\n";
	std::cout << "\nPress [c] for controls\n";
	std::cout << "\nPeter Piper picked a peck of pickled peppers.\n"
			  << "Did Peter Piper pick a peck of pickled peppers?\n"
			  << "If Peter Piper picked a peck of pickled peppers,\n"
			  << "where's the peck of pickled peppers Peter Piper picked? \n\n";


	auto& scene = SceneManager::GetInstance().CreateScene("Demo");
	const auto font = ResourceManager::GetInstance().LoadFont("Lingua.otf", 36);

	auto go = std::make_shared<GameObject>();
	
	go->AddComponent(new RenderComponent(go.get(), "BurgerTimeStage1.png"));
	scene.Add(go);

	//Add Fps
	auto fpsObject = std::make_shared<GameObject>();

	fpsObject->AddComponent(new RenderComponent(fpsObject.get()));
	auto fpsText = new TextComponent(fpsObject.get());
	fpsText->SetColor({ 0, 255, 0, 1 });
	fpsObject->AddComponent(fpsText);
	fpsObject->AddComponent(new FpsCounter(fpsObject.get()));
	fpsObject->SetLocalPosition(10, 10);
	scene.Add(fpsObject);

	//Add HealthUI
	auto HealthVisualsObject = std::make_shared<GameObject>();

	HealthVisualsObject->AddComponent(new RenderComponent(HealthVisualsObject.get()));
	const auto healthText = new TextComponent (HealthVisualsObject.get());
	healthText->SetColor({ 0, 255, 0, 1});
	HealthVisualsObject->AddComponent(healthText);
	const auto healthUI = new HealthUI(HealthVisualsObject.get());
	HealthVisualsObject->AddComponent(healthUI);
	HealthVisualsObject->SetLocalPosition({10, 500});
	scene.Add(HealthVisualsObject);

	//Add ScoreUI
	auto ScoreVisualsObject = std::make_shared<GameObject>();

	ScoreVisualsObject->AddComponent(new RenderComponent(ScoreVisualsObject.get()));
	const auto ScoreText = new TextComponent(ScoreVisualsObject.get());
	ScoreText->SetColor({ 0, 255, 0, 1 });
	ScoreVisualsObject->AddComponent(ScoreText);
	const auto scoreUI = new ScoreUI(ScoreVisualsObject.get());
	ScoreVisualsObject->AddComponent(scoreUI);
	ScoreVisualsObject->SetLocalPosition({ 10, 550 });
	scene.Add(ScoreVisualsObject);

	//Add Player
	auto playerObject = std::make_shared<GameObject>();
	playerObject->SetLocalPosition(100, 100);

	playerObject->AddComponent(new Subject(playerObject.get()));
	playerObject->AddComponent(new HealthComponent(playerObject.get(), 4, {healthUI}));
	playerObject->AddComponent(new ScoreComponent(playerObject.get(), { scoreUI, &CSteamAchievements::GetInstance() }));
	auto peterPepper = new character::PeterPepper(playerObject.get(), 50);
	playerObject->AddComponent(peterPepper);
	playerObject->AddComponent(new BoxCollider2d(playerObject.get(), {48, 48}, false, {peterPepper}));
	playerObject->AddComponent(new RenderComponent(playerObject.get(), "BurgerTimeSpriteSheet.png"));
	auto playerSprite = new SpriteRenderComponent(playerObject.get(), { 9,{0,0},{48,48} });
	playerSprite->AddAnimation(static_cast<int>(character::AnimationState::Idle), { 1, 1 });
	playerSprite->AddAnimation(static_cast<int>(character::AnimationState::RunHorizontal), { 3, 3 });
	playerSprite->AddAnimation(static_cast<int>(character::AnimationState::RunVertical), { 3, 6 });
	playerSprite->AddAnimation(static_cast<int>(character::AnimationState::PepperHorizontal), { 1, 10 });
	playerSprite->AddAnimation(static_cast<int>(character::AnimationState::PepperVertical), { 1, 11 });
	playerSprite->AddAnimation(static_cast<int>(character::AnimationState::Die), { 6, 12 });
	playerSprite->SetCurrentSprite(1);
	playerObject->AddComponent(playerSprite);

	scene.Add(playerObject);

	InputManager::GetInstance().MapActionKey({ ActionState::Up, ControllerButton::ButtonA, 0, SDLK_e },
		std::make_unique<DamagePlayer>(peterPepper));
	InputManager::GetInstance().MapActionKey({ ActionState::Up, ControllerButton::ButtonX },
		std::make_unique<IncreaseScore>(playerObject.get()));

	//Movement bindings
	InputManager::GetInstance().MapActionKey({ ActionState::Hold, ControllerButton::None, 0, SDLK_w },
		std::make_unique<MoveCommand>(peterPepper, MoveCommand::ActionDirection::Up));
	InputManager::GetInstance().MapActionKey({ ActionState::Hold, ControllerButton::None, 0, SDLK_a },
		std::make_unique<MoveCommand>(peterPepper, MoveCommand::ActionDirection::Left));
	InputManager::GetInstance().MapActionKey({ ActionState::Hold, ControllerButton::None, 0, SDLK_s },
		std::make_unique<MoveCommand>(peterPepper, MoveCommand::ActionDirection::Down));
	InputManager::GetInstance().MapActionKey({ ActionState::Hold, ControllerButton::None, 0, SDLK_d },
		std::make_unique<MoveCommand>(peterPepper, MoveCommand::ActionDirection::Right));

	InputManager::GetInstance().MapActionKey({ ActionState::ThumbL, ControllerButton::None, 0 },
		std::make_unique<MoveCommand>(peterPepper, MoveCommand::ActionDirection::All));

	//Add HealthUI P2
	auto HealthVisualsObject2 = std::make_shared<GameObject>();

	HealthVisualsObject2->AddComponent(new RenderComponent(HealthVisualsObject2.get()));
	const auto healthText2 = new TextComponent(HealthVisualsObject2.get());
	healthText2->SetColor({255, 255, 0, 1});
	HealthVisualsObject2->AddComponent(healthText2);
	const auto healthUI2 = new HealthUI(HealthVisualsObject2.get());
	HealthVisualsObject2->AddComponent(healthUI2);
	HealthVisualsObject2->SetLocalPosition({ 990, 500 });
	scene.Add(HealthVisualsObject2);

	//Add ScoreUI P2
	auto ScoreVisualsObject2 = std::make_shared<GameObject>();

	ScoreVisualsObject2->AddComponent(new RenderComponent(ScoreVisualsObject2.get()));
	const auto scoreText2 = new TextComponent(ScoreVisualsObject2.get());
	scoreText2->SetColor({ 255, 255, 0, 1});
	ScoreVisualsObject2->AddComponent(scoreText2);
	const auto scoreUI2 = new ScoreUI(ScoreVisualsObject2.get());
	ScoreVisualsObject2->AddComponent(scoreUI2);
	ScoreVisualsObject2->SetLocalPosition({ 890, 550 });
	scene.Add(ScoreVisualsObject2);
	
	//Add Player2
	auto playerObject2 = std::make_shared<GameObject>();
	playerObject2->SetLocalPosition(300, 100);

	playerObject2->AddComponent(new Subject(playerObject2.get()));
	playerObject2->AddComponent(new HealthComponent(playerObject2.get(), 4, {healthUI2}));
	playerObject2->AddComponent(new ScoreComponent(playerObject2.get(), {scoreUI2, &CSteamAchievements::GetInstance()}));
	auto peterPepper2 = new character::PeterPepper(playerObject2.get());
	playerObject2->AddComponent(peterPepper2);
	playerObject2->AddComponent(new BoxCollider2d(playerObject2.get(), { 50, 100 }, false, { peterPepper2 }));
	playerObject2->AddComponent(new RenderComponent(playerObject2.get(), "BurgerTimeSpriteSheet.png"));
	auto playerSprite2 = new SpriteRenderComponent(playerObject2.get(), { 9,{0,0},{48,48} });
	playerSprite2->AddAnimation(static_cast<int>(character::AnimationState::Idle), { 1, 1 });
	playerSprite2->AddAnimation(static_cast<int>(character::AnimationState::RunHorizontal), { 3, 3 });
	playerSprite2->AddAnimation(static_cast<int>(character::AnimationState::RunVertical), { 3, 6 });
	playerSprite2->AddAnimation(static_cast<int>(character::AnimationState::PepperHorizontal), { 1, 10 });
	playerSprite2->AddAnimation(static_cast<int>(character::AnimationState::PepperVertical), { 1, 11 });
	playerSprite2->AddAnimation(static_cast<int>(character::AnimationState::Die), { 6, 12 });
	playerSprite2->SetCurrentSprite(1);
	playerObject2->AddComponent(playerSprite2);
	scene.Add(playerObject2);

	InputManager::GetInstance().MapActionKey({ ActionState::Up, ControllerButton::ButtonB, 1 }, 
		std::make_unique<DamagePlayer>(peterPepper2));
	InputManager::GetInstance().MapActionKey({ ActionState::Up, ControllerButton::ButtonY, 1 },
		std::make_unique<IncreaseScore>(playerObject2.get()));

	//Movement bindings
	InputManager::GetInstance().MapActionKey({ ActionState::Hold, ControllerButton::None, 0, SDLK_i },
		std::make_unique<MoveCommand>(peterPepper2, MoveCommand::ActionDirection::Up));	
	InputManager::GetInstance().MapActionKey({ ActionState::Hold, ControllerButton::None, 0, SDLK_j },
		std::make_unique<MoveCommand>(peterPepper2, MoveCommand::ActionDirection::Left));
	InputManager::GetInstance().MapActionKey({ ActionState::Hold, ControllerButton::None, 0, SDLK_k },
		std::make_unique<MoveCommand>(peterPepper2, MoveCommand::ActionDirection::Down));
	InputManager::GetInstance().MapActionKey({ ActionState::Hold, ControllerButton::None, 0, SDLK_l },
		std::make_unique<MoveCommand>(peterPepper2, MoveCommand::ActionDirection::Right));

	InputManager::GetInstance().MapActionKey({ ActionState::ThumbL, ControllerButton::None, 1 },
		std::make_unique<MoveCommand>(peterPepper2, MoveCommand::ActionDirection::All));

	//testing walls
	auto wall = std::make_shared<GameObject>();
	wall->SetLocalPosition(300, 300);
	wall->AddComponent(new Subject(wall.get()));
	wall->AddComponent(new BoxCollider2d(wall.get(), { 48, 48 }, false, {}, b2_staticBody));
	scene.Add(wall);

	//Debug
	InputManager::GetInstance().MapActionKey({ ActionState::TriggerL, ControllerButton::None, 0 },
		std::make_unique<DebugFloatCTXCommand>());

	//InputManager::GetInstance().MapActionKey({ ActionState::ThumbL, ControllerButton::None, 0 },
	//	std::make_unique<DebugVec2CTXCommand>());

	InputManager::GetInstance().MapActionKey({ ActionState::Up, ControllerButton::Start, 0, SDLK_c },
		std::make_unique<PrintControls>());

	scene.Start();
}

void biggin::Biggin::Cleanup()
{
	Renderer::GetInstance().Destroy();
	SDL_DestroyWindow(m_Window);
	m_Window = nullptr;
	SDL_Quit();
}

void biggin::Biggin::Run()
{
	Initialize();

	// tell the resource manager where he can find the game data
	ResourceManager::GetInstance().Init("../Data/");
	Box2dManager::GetInstance().Init();

	LoadGame();

	{
		const auto& renderer = Renderer::GetInstance();
		const auto& sceneManager = SceneManager::GetInstance();
		const auto& input = InputManager::GetInstance();
		auto& gameTime = GameTime::GetInstance();
		auto& box2dManager = Box2dManager::GetInstance();

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
				box2dManager.Simulate();

				sceneManager.FixedUpdate();
				lag -= gameTime.GetFixedTimeStep();
			}
			renderer.Render();
		}
	}

	Cleanup();
}
