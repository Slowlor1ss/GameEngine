#include "BurgerTimeMenuState.h"
#include <SDL_keycode.h>
#include "BurgerTimeCommands.hpp"
#include "imgui.h"
#include "GameLoader.h"
#include <thread>
#include "BoxCollider2d.h"
#include "Burger.h"
#include "EnemySpawner.h"
#include "FpsCounter.h"
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
#include "MapLoader.h"
#include "PeterPepper.h"
#include "RenderComponent.h"
#include "ScoreComponent.h"
#include "ScoreUI.h"
#include "SoundServiceLocator.h"
#include "SpriteRenderComponent.h"
#include "StatsAndAchievements.h"
#include "Subject.h"
#include "InputManager.h"

using namespace biggin;
using namespace burgerTime;

MainMenuState* BurgerTimeMenuState::m_pMainMenuState{ new MainMenuState() };
RunningState* BurgerTimeMenuState::m_pRunningState{ new RunningState() };
OptionsState* BurgerTimeMenuState::m_pOptionsState{ new OptionsState() };

void MainMenuState::Enter()
{
	SceneManager::GetInstance().ChangeActiveScene("MainMenu");
}

void MainMenuState::RenderMenu(GameLoader* UiMenu)
{
	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoBackground;
	window_flags |= ImGuiWindowFlags_NoTitleBar;
	window_flags |= ImGuiWindowFlags_NoResize;
	window_flags |= ImGuiWindowFlags_AlwaysAutoResize;

	ImGui::Begin("MainMenu", &m_IsOpen, window_flags);
	ImGui::SetItemDefaultFocus();
	if (ImGui::Button("SinglePlayer", { 200,50 }))
	{
		if (!m_IsLoadedSinglePlayer)
			LoadSinglePlayer();

		SceneManager::GetInstance().ChangeActiveScene("SinglePlayer");

		UiMenu->GetState()->Exit();
		UiMenu->SetState(m_pRunningState);
		UiMenu->GetState()->Enter();
	}

	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();
	ImGui::Spacing();

	if (ImGui::Button("To be added", { 200,50 }))
	{
		/*if (!m_IsLoadedMultiPlayer)
			LoadMultiPlayer();

		m_pState->Exit();
		m_pState = m_pRunningState;
		m_pState->Enter();*/
	}

	ImGui::End();
}

void MainMenuState::LoadSinglePlayer()
{
	//TODO: just for testing will move this to a correct location later
	SoundServiceLocator::GetSoundSystem().Play("main.wav", 0.2f, false);

	std::cout << "==============================================================================\r\n     __________  ____  ____ _________    ________  __________ _________\r\n     |   ___   \\ |  |  |  | |   ___  \\  /  ______| | _______| |   ___  \\\r\n     |   |_|    ||  |  |  | |   |_|   ||  /        | |        |   |_|   |\r\n     |         / |  |  |  | | ___  __/ | |   ____  | |__      | ___  __/\r\n     |   ___   \\ |  \\__/  | | |  \\ \\   | |  |___ | | ___|     | |  \\ \\\r\n     |   |_|    ||        | | |   \\ \\  | |_____| | | |_______ | |   \\ \\\r\n     |_________/  \\______/  |_|    \\_\\  \\_______/  |________| |_|    \\_\\ \r\n\r\n==============================================================================\r\n                __________ __________ ___    ___ __________\r\n                |___  ___| |___  ___| |  \\  /  | | _______|\r\n                   |  |       |  |    |   \\/   | | |\r\n                   |  |       |  |    |        | | |__\r\n                   |  |       |  |    |  \\  /  | | ___|\r\n                   |  |    ___|  |___ |  |\\/|  | | |_______\r\n                   |__|    |________| |__|  |__| |________|\r\n\r\n==============================================================================\r\n\t\t\t+=+=+=+=+=+=+=+=+=+=+=+=+=+=+\r\n==============================================================================\n";
	std::cout << "\nPress [c] for controls\n";
	std::cout << "\nPeter Piper picked a peck of pickled peppers.\n"
		<< "Did Peter Piper pick a peck of pickled peppers?\n"
		<< "If Peter Piper picked a peck of pickled peppers,\n"
		<< "where's the peck of pickled peppers Peter Piper picked? \n\n";

	auto& sceneManager = SceneManager::GetInstance();
	auto& scene = sceneManager.CreateScene("SinglePlayer");
	sceneManager.ChangeActiveScene("SinglePlayer");
	const auto font = ResourceManager::GetInstance().LoadFont("Lingua.otf", 36);

	//Load map
	auto map = std::make_shared<GameObject>();
	map->Setname("Map");
	map->SetLocalPosition(9 * MapLoader::GetGridSize(), 0 * MapLoader::GetGridSize());
	map->AddComponent(new Subject(map.get()));
	//map->AddComponent(new BoxCollider2d(map.get(), { 1, 1000 }, false, b2_staticBody, {}, {}, {0,1000 }, false));
	//map->AddComponent(new BoxCollider2d(map.get(), { 1, 1000 }, false, b2_staticBody, {}, {}, {39 * MapLoader::GetGridSize(), 1000 }, false));
	map->AddComponent(new RenderComponent(map.get()));
	map->AddComponent(new MapLoader(map.get(), { m_pRunningState }));

	scene.Add(map);
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
	const auto healthText = new TextComponent(HealthVisualsObject.get());
	healthText->SetColor({ 0, 255, 0, 1 });
	HealthVisualsObject->AddComponent(healthText);
	const auto healthUI = new HealthUI(HealthVisualsObject.get());
	HealthVisualsObject->AddComponent(healthUI);
	HealthVisualsObject->SetLocalPosition({ 10, 500 });
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
	playerObject->Setname("Player");


	playerObject->AddComponent(new Subject(playerObject.get()));
	playerObject->AddComponent(new HealthComponent(playerObject.get(), 4, { healthUI }));
	playerObject->AddComponent(new ScoreComponent(playerObject.get(), { scoreUI, &CSteamAchievements::GetInstance() }));
	auto peterPepper = new character::PeterPepper(playerObject.get(), 100);
	playerObject->AddComponent(peterPepper);
	b2Filter filterPlayer{};
	filterPlayer.maskBits = 0xFFFF ^ character::PeterPepper::PlayerCollisionGroup::playerIgnoreGroup; //Ignore group 4
	filterPlayer.categoryBits = character::PeterPepper::PlayerCollisionGroup::playerCollisionGroup; //set self to group 5
	playerObject->AddComponent(new BoxCollider2d(playerObject.get(), {20, 30}, false, b2_dynamicBody, {peterPepper},
	                                             "Player", {}, true, filterPlayer));
	playerObject->AddComponent(new RenderComponent(playerObject.get(), "BurgerTimeSpriteSheet.png"));
	auto playerSprite = new SpriteRenderComponent(playerObject.get(), { 9,{0,0},{32,32} });
	playerSprite->AddAnimation(static_cast<int>(character::AnimationState::Idle), { 1, 1 });
	playerSprite->AddAnimation(static_cast<int>(character::AnimationState::RunHorizontal), { 3, 3 });
	playerSprite->AddAnimation(static_cast<int>(character::AnimationState::RunVertical), { 3, 6 });
	playerSprite->AddAnimation(static_cast<int>(character::AnimationState::PepperHorizontal), { 1, 10 });
	playerSprite->AddAnimation(static_cast<int>(character::AnimationState::PepperVertical), { 1, 11 });
	playerSprite->AddAnimation(static_cast<int>(character::AnimationState::Die), { 6, 12 });
	playerSprite->SetCurrentSprite(1);
	playerObject->AddComponent(playerSprite);

	scene.Add(playerObject);

	//InputManager::GetInstance().MapActionKey({ ActionState::Up, ControllerButton::ButtonA, 0, SDLK_e },
	//	std::make_unique<DamagePlayer>(peterPepper));
	//InputManager::GetInstance().MapActionKey({ ActionState::Up, ControllerButton::ButtonX },
	//	std::make_unique<IncreaseScore>(playerObject.get()));

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

	////Add HealthUI P2
	//auto HealthVisualsObject2 = std::make_shared<GameObject>();
	//
	//HealthVisualsObject2->AddComponent(new RenderComponent(HealthVisualsObject2.get()));
	//const auto healthText2 = new TextComponent(HealthVisualsObject2.get());
	//healthText2->SetColor({ 255, 255, 0, 1 });
	//HealthVisualsObject2->AddComponent(healthText2);
	//const auto healthUI2 = new HealthUI(HealthVisualsObject2.get());
	//HealthVisualsObject2->AddComponent(healthUI2);
	//HealthVisualsObject2->SetLocalPosition({ 990, 500 });
	//scene.Add(HealthVisualsObject2);
	//
	////Add ScoreUI P2
	//auto ScoreVisualsObject2 = std::make_shared<GameObject>();
	//
	//ScoreVisualsObject2->AddComponent(new RenderComponent(ScoreVisualsObject2.get()));
	//const auto scoreText2 = new TextComponent(ScoreVisualsObject2.get());
	//scoreText2->SetColor({ 255, 255, 0, 1 });
	//ScoreVisualsObject2->AddComponent(scoreText2);
	//const auto scoreUI2 = new ScoreUI(ScoreVisualsObject2.get());
	//ScoreVisualsObject2->AddComponent(scoreUI2);
	//ScoreVisualsObject2->SetLocalPosition({ 890, 550 });
	//scene.Add(ScoreVisualsObject2);

	//auto test = std::make_shared<GameObject>();
	//test->AddComponent(new Subject(test.get()));
	//test->AddComponent(new BoxCollider2d(test.get(), { 30, 30 }, false, b2_staticBody, { }, "", {}, false));
	//test->AddComponent(new RenderComponent(test.get(), "test.png"));
	//scene.Add(test);

	//Add Player2
	//auto playerObject2 = std::make_shared<GameObject>();
	//playerObject2->SetLocalPosition(300, 100);
	//
	//playerObject2->AddComponent(new Subject(playerObject2.get()));
	//playerObject2->AddComponent(new HealthComponent(playerObject2.get(), 4, {healthUI2}));
	//playerObject2->AddComponent(new ScoreComponent(playerObject2.get(), {scoreUI2, &CSteamAchievements::GetInstance()}));
	//auto peterPepper2 = new character::PeterPepper(playerObject2.get());
	//playerObject2->AddComponent(peterPepper2);
	//playerObject2->AddComponent(new BoxCollider2d(playerObject2.get(), { 50, 100 }, false, { peterPepper2 }));
	//playerObject2->AddComponent(new RenderComponent(playerObject2.get(), "BurgerTimeSpriteSheet.png"));
	//auto playerSprite2 = new SpriteRenderComponent(playerObject2.get(), { 9,{0,0},{48,48} });
	//playerSprite2->AddAnimation(static_cast<int>(character::AnimationState::Idle), { 1, 1 });
	//playerSprite2->AddAnimation(static_cast<int>(character::AnimationState::RunHorizontal), { 3, 3 });
	//playerSprite2->AddAnimation(static_cast<int>(character::AnimationState::RunVertical), { 3, 6 });
	//playerSprite2->AddAnimation(static_cast<int>(character::AnimationState::PepperHorizontal), { 1, 10 });
	//playerSprite2->AddAnimation(static_cast<int>(character::AnimationState::PepperVertical), { 1, 11 });
	//playerSprite2->AddAnimation(static_cast<int>(character::AnimationState::Die), { 6, 12 });
	//playerSprite2->SetCurrentSprite(1);
	//playerObject2->AddComponent(playerSprite2);
	//scene.Add(playerObject2);
	//
	//InputManager::GetInstance().MapActionKey({ ActionState::Up, ControllerButton::ButtonB, 1 }, 
	//	std::make_unique<DamagePlayer>(peterPepper2));
	//InputManager::GetInstance().MapActionKey({ ActionState::Up, ControllerButton::ButtonY, 1 },
	//	std::make_unique<IncreaseScore>(playerObject2.get()));
	//
	////Movement bindings
	//InputManager::GetInstance().MapActionKey({ ActionState::Hold, ControllerButton::None, 0, SDLK_i },
	//	std::make_unique<MoveCommand>(peterPepper2, MoveCommand::ActionDirection::Up));	
	//InputManager::GetInstance().MapActionKey({ ActionState::Hold, ControllerButton::None, 0, SDLK_j },
	//	std::make_unique<MoveCommand>(peterPepper2, MoveCommand::ActionDirection::Left));
	//InputManager::GetInstance().MapActionKey({ ActionState::Hold, ControllerButton::None, 0, SDLK_k },
	//	std::make_unique<MoveCommand>(peterPepper2, MoveCommand::ActionDirection::Down));
	//InputManager::GetInstance().MapActionKey({ ActionState::Hold, ControllerButton::None, 0, SDLK_l },
	//	std::make_unique<MoveCommand>(peterPepper2, MoveCommand::ActionDirection::Right));
	//
	//InputManager::GetInstance().MapActionKey({ ActionState::ThumbL, ControllerButton::None, 1 },
	//	std::make_unique<MoveCommand>(peterPepper2, MoveCommand::ActionDirection::All));
	//
	//testing walls
	//auto wall = std::make_shared<GameObject>();
	//
	//wall->AddComponent(new Subject(wall.get()));
	//wall->AddComponent(new BoxCollider2d(wall.get(), { 48, 48 }, false, {}, b2_staticBody));
	//wall->AddComponent(new BoxCollider2d(wall.get(), { 48, 48 }, false, {}, b2_staticBody, { 0, 48 }));
	//wall->AddComponent(new BoxCollider2d(wall.get(), { 48, 48 }, false, {}, b2_staticBody, { 0, 48 * 2 }));
	//wall->SetLocalPosition(150, 100);
	//scene.Add(wall);
	//
	//Debug
	//InputManager::GetInstance().MapActionKey({ ActionState::TriggerL, ControllerButton::None, 0 },
	//	std::make_unique<DebugFloatCTXCommand>());

	InputManager::GetInstance().MapActionKey({biggin::ActionState::Up, biggin::ControllerButton::Start, 0, SDLK_c },
		std::make_unique<PrintControls>());


	auto enemySpawner = std::make_shared<GameObject>();
	enemySpawner->Setname("EnemySpawner");
	enemySpawner->AddComponent(new EnemySpawner(enemySpawner.get()));
	scene.Add(enemySpawner);


	////Foreground
	//auto foreground = std::make_shared<GameObject>();
	//foreground->AddComponent(new RenderComponent(foreground.get(), "BurgerTimeStageForeground.png"));
	//scene.Add(foreground);

	scene.Start();

	m_IsLoadedSinglePlayer = true;
}

void RunningState::Enter()
{

}

void RunningState::OnNotify(biggin::Component* /*entity*/, const std::string& event)
{
	if (event == "FinishedLevel")
	{
		//create new scene
		//move player, score, and health
		//reset/remove old scene
	}
	else if (event == "GameOver")
	{
		//save high-score
		//reset all
		//load main menu
	}
}

void RunningState::RenderMenu(GameLoader* UiMenu)
{
	if (InputManager::GetInstance().IsPressed(SDLK_ESCAPE, ControllerButton::Start, 0)
		|| InputManager::GetInstance().IsPressed(SDLK_UNKNOWN, ControllerButton::Start, 1))
	{
		UiMenu->GetState()->Exit();
		UiMenu->SetState(m_pOptionsState);
		UiMenu->GetState()->Enter();
	}
}

void OptionsState::Enter()
{
	SceneManager::GetInstance().SetScenesPaused(true);
}

void OptionsState::Exit()
{
	SceneManager::GetInstance().SetScenesPaused(false);
}

void OptionsState::RenderMenu(GameLoader* UiMenu)
{
	if (InputManager::GetInstance().IsPressed(SDLK_ESCAPE ,ControllerButton::Start, 0)
		|| InputManager::GetInstance().IsPressed(SDLK_UNKNOWN, ControllerButton::Start, 1))
	{
		UiMenu->GetState()->Exit();
		UiMenu->SetState(m_pRunningState);
		UiMenu->GetState()->Enter();
	}

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoBackground;
	window_flags |= ImGuiWindowFlags_NoTitleBar;
	window_flags |= ImGuiWindowFlags_NoResize;
	window_flags |= ImGuiWindowFlags_AlwaysAutoResize;
	//window_flags |= ImGuiWindowFlags_NavFlattened;

	ImGui::Begin("Options", &m_IsOpen, window_flags);
	static std::string test{ "MainMenu" };
	if (ImGui::Button(test.c_str(), { 200,50 }))
	{
		UiMenu->GetState()->Exit();
		UiMenu->SetState(m_pMainMenuState);
		UiMenu->GetState()->Enter();
	}

	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();
	ImGui::Spacing();

	if (ImGui::Button("Resume", { 200,50 }))
	{
		UiMenu->GetState()->Exit();
		UiMenu->SetState(m_pRunningState);
		UiMenu->GetState()->Enter();
	}

	ImGui::End();
}

void BurgerTimeMenuState::Cleanup()
{
	delete m_pMainMenuState;
	m_pMainMenuState = nullptr;
	delete m_pRunningState;
	m_pRunningState = nullptr;
	delete m_pOptionsState;
	m_pOptionsState = nullptr;
}
