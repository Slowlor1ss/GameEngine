#include "BurgerTimeMenuState.h"
#include <SDL_keycode.h>
#include "BurgerTimeCommands.hpp"
#include "imgui.h"
#include "GameLoader.h"
#include <thread>
#include "Biggin.h"
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
#include "HighScoreUI.h"
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
#include "PepperShooter.h"
#include "PepperUI.h"

using namespace biggin;
using namespace burgerTime;

MainMenuState* BurgerTimeMenuState::m_pMainMenuState{ new MainMenuState() };
RunningState* BurgerTimeMenuState::m_pRunningState{ new RunningState() };
OptionsState* BurgerTimeMenuState::m_pOptionsState{ new OptionsState() };
ConfirmationState* BurgerTimeMenuState::m_pConfirmationState{ new ConfirmationState() };
HighScoreState* BurgerTimeMenuState::m_pHighScoreState{ new HighScoreState() };
BurgerTimeMenuState* BurgerTimeMenuState::m_pPrevState{ nullptr };



void BurgerTimeMenuState::Cleanup()
{
	delete m_pMainMenuState;
	m_pMainMenuState = nullptr;
	delete m_pRunningState;
	m_pRunningState = nullptr;
	delete m_pOptionsState;
	m_pOptionsState = nullptr;
}

void BurgerTimeMenuState::ChangeState(BurgerTimeMenuState*& currState, BurgerTimeMenuState* nextState)
{
	currState->Exit();
	m_pPrevState = currState;
	currState = nextState;
	currState->Enter();
}

void MainMenuState::Enter()
{
	SceneManager::GetInstance().ChangeActiveScene("MainMenu");
	SoundServiceLocator::GetSoundSystem().Play("burgermenu.wav", 0.2f, false);
}

void MainMenuState::RenderMenu(BurgerTimeMenuState*& currState)
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
		SoundServiceLocator::GetSoundSystem().Play("menu_select.wav", 0.2f);

		LoadSinglePlayer();

		SceneManager::GetInstance().ChangeActiveScene("SinglePlayer");

		ChangeState(currState, m_pRunningState);
	}

	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();
	ImGui::Spacing();

	if (ImGui::Button("Coop", { 200,50 }))
	{
		SoundServiceLocator::GetSoundSystem().Play("menu_select.wav", 0.2f);

		LoadCoop();

		SceneManager::GetInstance().ChangeActiveScene("Coop");

		ChangeState(currState, m_pRunningState);
	}

	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();
	ImGui::Spacing();

	if (ImGui::Button("Versus", { 200,50 }))
	{
		SoundServiceLocator::GetSoundSystem().Play("menu_select.wav", 0.2f);

		LoadVersus();

		SceneManager::GetInstance().ChangeActiveScene("Versus");

		ChangeState(currState, m_pRunningState);
	}

	ImGui::End();
}

void MainMenuState::LoadSinglePlayer()
{
	SoundServiceLocator::GetSoundSystem().Play("level_intro.wav", 0.2f, false);

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

#ifdef _DEBUG
	//Add Fps
	auto fpsObject = std::make_shared<GameObject>();

	fpsObject->AddComponent(new RenderComponent(fpsObject.get()));
	auto fpsText = new TextComponent(fpsObject.get());
	fpsText->SetColor({ 0, 255, 0, 1 });
	fpsObject->AddComponent(fpsText);
	fpsObject->AddComponent(new FpsCounter(fpsObject.get()));
	fpsObject->SetLocalPosition(10, 10);
	scene.Add(fpsObject);
#endif

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
	ScoreText->SetColor({ 255, 0, 0, 1 });
	ScoreVisualsObject->AddComponent(ScoreText);
	const auto scoreUI = new ScoreUI(ScoreVisualsObject.get());
	ScoreVisualsObject->AddComponent(scoreUI);
	ScoreVisualsObject->SetLocalPosition({ 10, 550 });
	scene.Add(ScoreVisualsObject);

	//Add PepperUI
	auto PepperVisualsObject = std::make_shared<GameObject>();

	PepperVisualsObject->AddComponent(new RenderComponent(PepperVisualsObject.get()));
	const auto PepperText = new TextComponent(PepperVisualsObject.get());
	PepperText->SetColor({ 0, 255, 0, 1 });
	PepperVisualsObject->AddComponent(PepperText);
	const auto pepperUI = new PepperUI(PepperVisualsObject.get());
	PepperVisualsObject->AddComponent(pepperUI);
	PepperVisualsObject->SetLocalPosition({ Biggin::GetWindowWidth() - 125, 10 });
	scene.Add(PepperVisualsObject);

	//Add Player
	auto playerObject = std::make_shared<GameObject>();
	playerObject->Setname("Player");

	playerObject->AddComponent(new Subject(playerObject.get()));
	playerObject->AddComponent(new HealthComponent(playerObject.get(), 3, { healthUI }));
	playerObject->AddComponent(new ScoreComponent(playerObject.get(), { scoreUI, &CSteamAchievements::GetInstance() }));
	playerObject->AddComponent(new PepperShooter(playerObject.get(), { pepperUI }, 5));
	auto movementCompoent = new PossessGameObjectComponent(playerObject.get(), 75);
	playerObject->AddComponent(movementCompoent);
	auto peterPepper = new character::PeterPepper(playerObject.get());
	playerObject->AddComponent(peterPepper);
	b2Filter filterPlayer{};
	filterPlayer.maskBits = 0xFFFF ^ character::PeterPepper::PlayerCollisionGroup::playerIgnoreGroup; //Ignore group 4
	filterPlayer.categoryBits = character::PeterPepper::PlayerCollisionGroup::playerCollisionGroup; //set self to group 5
	playerObject->AddComponent(new BoxCollider2d(playerObject.get(), {20, 30}, false, b2_dynamicBody, {},
	                                             "Player", {}, true, filterPlayer));
	playerObject->AddComponent(new RenderComponent(playerObject.get(), "BurgerTimeSpriteSheet.png"));
	auto playerSprite = new SpriteRenderComponent(playerObject.get(), { 9,{0,0},{32,32} }, 0.1f);
	playerSprite->AddAnimation(static_cast<int>(character::AnimationState::Idle), { 1, 1 });
	playerSprite->AddAnimation(static_cast<int>(character::AnimationState::RunHorizontal), { 3, 3 });
	playerSprite->AddAnimation(static_cast<int>(character::AnimationState::RunVertical), { 3, 6 });
	playerSprite->AddAnimation(static_cast<int>(character::AnimationState::PepperHorizontal), { 1, 10 });
	playerSprite->AddAnimation(static_cast<int>(character::AnimationState::PepperVertical), { 1, 11 });
	playerSprite->AddAnimation(static_cast<int>(character::AnimationState::Die), { 6, 12 });
	playerSprite->SetCurrentSprite(1);
	playerObject->AddComponent(playerSprite);

	scene.Add(playerObject);

	//Movement bindings
	InputManager::GetInstance().MapActionKey({ ActionState::Hold, ControllerButton::None, 0, SDLK_w },
		std::make_unique<MoveCommand>(movementCompoent, MoveCommand::ActionDirection::Up));
	InputManager::GetInstance().MapActionKey({ ActionState::Hold, ControllerButton::None, 0, SDLK_a },
		std::make_unique<MoveCommand>(movementCompoent, MoveCommand::ActionDirection::Left));
	InputManager::GetInstance().MapActionKey({ ActionState::Hold, ControllerButton::None, 0, SDLK_s },
		std::make_unique<MoveCommand>(movementCompoent, MoveCommand::ActionDirection::Down));
	InputManager::GetInstance().MapActionKey({ ActionState::Hold, ControllerButton::None, 0, SDLK_d },
		std::make_unique<MoveCommand>(movementCompoent, MoveCommand::ActionDirection::Right));

	InputManager::GetInstance().MapActionKey({ ActionState::ThumbL, ControllerButton::None, 0 },
		std::make_unique<MoveCommand>(movementCompoent, MoveCommand::ActionDirection::All));

	InputManager::GetInstance().MapActionKey({ ActionState::Down, ControllerButton::ButtonA, 0, SDLK_SPACE },
		std::make_unique<ShootCommand>(peterPepper));

	auto enemySpawner = std::make_shared<GameObject>();
	enemySpawner->Setname("EnemySpawner");
	enemySpawner->AddComponent(new enemy::EnemySpawner(enemySpawner.get()));
	scene.Add(enemySpawner);

	scene.Start();
}

void MainMenuState::LoadCoop()
{
	SoundServiceLocator::GetSoundSystem().Play("level_intro.wav", 0.2f, false);

	auto& sceneManager = SceneManager::GetInstance();
	auto& scene = sceneManager.CreateScene("Coop");
	sceneManager.ChangeActiveScene("Coop");
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

#ifdef _DEBUG
	//Add Fps
	auto fpsObject = std::make_shared<GameObject>();

	fpsObject->AddComponent(new RenderComponent(fpsObject.get()));
	auto fpsText = new TextComponent(fpsObject.get());
	fpsText->SetColor({ 0, 255, 0, 1 });
	fpsObject->AddComponent(fpsText);
	fpsObject->AddComponent(new FpsCounter(fpsObject.get()));
	fpsObject->SetLocalPosition(10, 10);
	scene.Add(fpsObject);
#endif

#pragma region Player1
	{
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
		ScoreText->SetColor({ 255, 0, 0, 1 });
		ScoreVisualsObject->AddComponent(ScoreText);
		const auto scoreUI = new ScoreUI(ScoreVisualsObject.get());
		ScoreVisualsObject->AddComponent(scoreUI);
		ScoreVisualsObject->SetLocalPosition({ 10, 550 });
		scene.Add(ScoreVisualsObject);

		//Add PepperUI
		auto PepperVisualsObject = std::make_shared<GameObject>();

		PepperVisualsObject->AddComponent(new RenderComponent(PepperVisualsObject.get()));
		const auto PepperText = new TextComponent(PepperVisualsObject.get());
		PepperText->SetColor({ 0, 255, 0, 1 });
		PepperVisualsObject->AddComponent(PepperText);
		const auto pepperUI = new PepperUI(PepperVisualsObject.get());
		PepperVisualsObject->AddComponent(pepperUI);
		PepperVisualsObject->SetLocalPosition({ 10, 10 });
		scene.Add(PepperVisualsObject);

		//Add Player
		auto playerObject = std::make_shared<GameObject>();
		playerObject->Setname("Player");

		playerObject->AddComponent(new Subject(playerObject.get()));
		playerObject->AddComponent(new HealthComponent(playerObject.get(), 3, { healthUI }));
		playerObject->AddComponent(new ScoreComponent(playerObject.get(), { scoreUI, &CSteamAchievements::GetInstance() }));
		playerObject->AddComponent(new PepperShooter(playerObject.get(), { pepperUI }, 5));
		auto movementCompoent = new PossessGameObjectComponent(playerObject.get(), 75);
		playerObject->AddComponent(movementCompoent);
		auto peterPepper = new character::PeterPepper(playerObject.get());
		playerObject->AddComponent(peterPepper);
		b2Filter filterPlayer{};
		filterPlayer.maskBits = 0xFFFF ^ character::PeterPepper::PlayerCollisionGroup::playerIgnoreGroup; //Ignore group 4
		filterPlayer.categoryBits = character::PeterPepper::PlayerCollisionGroup::playerCollisionGroup; //set self to group 5
		filterPlayer.groupIndex = -1; //ignore everything on index -1 this is to make sure players don't collide with each other
		playerObject->AddComponent(new BoxCollider2d(playerObject.get(), { 20, 30 }, false, b2_dynamicBody, {},
			"Player", {}, true, filterPlayer));
		playerObject->AddComponent(new RenderComponent(playerObject.get(), "BurgerTimeSpriteSheet.png"));
		auto playerSprite = new SpriteRenderComponent(playerObject.get(), { 9,{0,0},{32,32} }, 0.1f);
		playerSprite->AddAnimation(static_cast<int>(character::AnimationState::Idle), { 1, 1 });
		playerSprite->AddAnimation(static_cast<int>(character::AnimationState::RunHorizontal), { 3, 3 });
		playerSprite->AddAnimation(static_cast<int>(character::AnimationState::RunVertical), { 3, 6 });
		playerSprite->AddAnimation(static_cast<int>(character::AnimationState::PepperHorizontal), { 1, 10 });
		playerSprite->AddAnimation(static_cast<int>(character::AnimationState::PepperVertical), { 1, 11 });
		playerSprite->AddAnimation(static_cast<int>(character::AnimationState::Die), { 6, 12 });
		playerSprite->SetCurrentSprite(1);
		playerObject->AddComponent(playerSprite);

		scene.Add(playerObject);

		//Movement bindings
		InputManager::GetInstance().MapActionKey({ ActionState::Hold, ControllerButton::None, 0, SDLK_w },
			std::make_unique<MoveCommand>(movementCompoent, MoveCommand::ActionDirection::Up));
		InputManager::GetInstance().MapActionKey({ ActionState::Hold, ControllerButton::None, 0, SDLK_a },
			std::make_unique<MoveCommand>(movementCompoent, MoveCommand::ActionDirection::Left));
		InputManager::GetInstance().MapActionKey({ ActionState::Hold, ControllerButton::None, 0, SDLK_s },
			std::make_unique<MoveCommand>(movementCompoent, MoveCommand::ActionDirection::Down));
		InputManager::GetInstance().MapActionKey({ ActionState::Hold, ControllerButton::None, 0, SDLK_d },
			std::make_unique<MoveCommand>(movementCompoent, MoveCommand::ActionDirection::Right));

		InputManager::GetInstance().MapActionKey({ ActionState::ThumbL, ControllerButton::None, 0 },
			std::make_unique<MoveCommand>(movementCompoent, MoveCommand::ActionDirection::All));

		InputManager::GetInstance().MapActionKey({ ActionState::Down, ControllerButton::ButtonA, 0, SDLK_SPACE },
			std::make_unique<ShootCommand>(peterPepper));
	}
#pragma endregion


#pragma region Player2
	{
		//Add HealthUI
		auto HealthVisualsObject = std::make_shared<GameObject>();

		HealthVisualsObject->AddComponent(new RenderComponent(HealthVisualsObject.get()));
		const auto healthText = new TextComponent(HealthVisualsObject.get());
		healthText->SetColor({ 0, 255, 0, 1 });
		HealthVisualsObject->AddComponent(healthText);
		const auto healthUI = new HealthUI(HealthVisualsObject.get());
		HealthVisualsObject->AddComponent(healthUI);
		HealthVisualsObject->SetLocalPosition({ Biggin::GetWindowWidth() - 125, 500 });
		scene.Add(HealthVisualsObject);

		//Add ScoreUI
		auto ScoreVisualsObject = std::make_shared<GameObject>();

		ScoreVisualsObject->AddComponent(new RenderComponent(ScoreVisualsObject.get()));
		const auto ScoreText = new TextComponent(ScoreVisualsObject.get());
		ScoreText->SetColor({ 255, 0, 0, 1 });
		ScoreVisualsObject->AddComponent(ScoreText);
		const auto scoreUI = new ScoreUI(ScoreVisualsObject.get());
		ScoreVisualsObject->AddComponent(scoreUI);
		ScoreVisualsObject->SetLocalPosition({ Biggin::GetWindowWidth() - 125, 550 });
		scene.Add(ScoreVisualsObject);

		//Add PepperUI
		auto PepperVisualsObject = std::make_shared<GameObject>();

		PepperVisualsObject->AddComponent(new RenderComponent(PepperVisualsObject.get()));
		const auto PepperText = new TextComponent(PepperVisualsObject.get());
		PepperText->SetColor({ 0, 255, 0, 1 });
		PepperVisualsObject->AddComponent(PepperText);
		const auto pepperUI = new PepperUI(PepperVisualsObject.get());
		PepperVisualsObject->AddComponent(pepperUI);
		PepperVisualsObject->SetLocalPosition({ Biggin::GetWindowWidth() - 125, 10 });
		scene.Add(PepperVisualsObject);

		//Add Player
		auto playerObject = std::make_shared<GameObject>();
		playerObject->Setname("Player");

		playerObject->AddComponent(new Subject(playerObject.get()));
		playerObject->AddComponent(new HealthComponent(playerObject.get(), 3, { healthUI }));
		playerObject->AddComponent(new ScoreComponent(playerObject.get(), { scoreUI, &CSteamAchievements::GetInstance() }));
		playerObject->AddComponent(new PepperShooter(playerObject.get(), { pepperUI }, 5));
		auto movementCompoent = new PossessGameObjectComponent(playerObject.get(), 75);
		playerObject->AddComponent(movementCompoent);
		auto peterPepper = new character::PeterPepper(playerObject.get());
		playerObject->AddComponent(peterPepper);
		b2Filter filterPlayer{};
		filterPlayer.maskBits = 0xFFFF ^ character::PeterPepper::PlayerCollisionGroup::playerIgnoreGroup; //Ignore group 4
		filterPlayer.categoryBits = character::PeterPepper::PlayerCollisionGroup::playerCollisionGroup; //set self to group 5
		filterPlayer.groupIndex = -1; //ignore everything on index -1 this is to make sure players don't collide with each other
		playerObject->AddComponent(new BoxCollider2d(playerObject.get(), { 20, 30 }, false, b2_dynamicBody, { },
			"Player", {}, true, filterPlayer));
		playerObject->AddComponent(new RenderComponent(playerObject.get(), "BurgerTimeSpriteSheet.png"));
		auto playerSprite = new SpriteRenderComponent(playerObject.get(), { 9,{0,0},{32,32} }, 0.1f);
		playerSprite->AddAnimation(static_cast<int>(character::AnimationState::Idle), { 1, 1 });
		playerSprite->AddAnimation(static_cast<int>(character::AnimationState::RunHorizontal), { 3, 3 });
		playerSprite->AddAnimation(static_cast<int>(character::AnimationState::RunVertical), { 3, 6 });
		playerSprite->AddAnimation(static_cast<int>(character::AnimationState::PepperHorizontal), { 1, 10 });
		playerSprite->AddAnimation(static_cast<int>(character::AnimationState::PepperVertical), { 1, 11 });
		playerSprite->AddAnimation(static_cast<int>(character::AnimationState::Die), { 6, 12 });
		playerSprite->SetCurrentSprite(1);
		playerObject->AddComponent(playerSprite);

		scene.Add(playerObject);

		//Movement bindings
		InputManager::GetInstance().MapActionKey({ ActionState::Hold, ControllerButton::None, 1, SDLK_UP },
			std::make_unique<MoveCommand>(movementCompoent, MoveCommand::ActionDirection::Up));
		InputManager::GetInstance().MapActionKey({ ActionState::Hold, ControllerButton::None, 1, SDLK_LEFT },
			std::make_unique<MoveCommand>(movementCompoent, MoveCommand::ActionDirection::Left));
		InputManager::GetInstance().MapActionKey({ ActionState::Hold, ControllerButton::None, 1, SDLK_DOWN },
			std::make_unique<MoveCommand>(movementCompoent, MoveCommand::ActionDirection::Down));
		InputManager::GetInstance().MapActionKey({ ActionState::Hold, ControllerButton::None, 1, SDLK_RIGHT },
			std::make_unique<MoveCommand>(movementCompoent, MoveCommand::ActionDirection::Right));

		InputManager::GetInstance().MapActionKey({ ActionState::ThumbL, ControllerButton::None, 1 },
			std::make_unique<MoveCommand>(movementCompoent, MoveCommand::ActionDirection::All));

		InputManager::GetInstance().MapActionKey({ ActionState::Down, ControllerButton::ButtonA, 1, SDLK_RSHIFT },
			std::make_unique<ShootCommand>(peterPepper));
	}
#pragma endregion

	auto enemySpawner = std::make_shared<GameObject>();
	enemySpawner->Setname("EnemySpawner");
	enemySpawner->AddComponent(new enemy::EnemySpawner(enemySpawner.get()));
	scene.Add(enemySpawner);

	scene.Start();
}

void MainMenuState::LoadVersus()
{
	SoundServiceLocator::GetSoundSystem().Play("level_intro.wav", 0.2f, false);

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

#ifdef _DEBUG
	//Add Fps
	auto fpsObject = std::make_shared<GameObject>();

	fpsObject->AddComponent(new RenderComponent(fpsObject.get()));
	auto fpsText = new TextComponent(fpsObject.get());
	fpsText->SetColor({ 0, 255, 0, 1 });
	fpsObject->AddComponent(fpsText);
	fpsObject->AddComponent(new FpsCounter(fpsObject.get()));
	fpsObject->SetLocalPosition(10, 10);
	scene.Add(fpsObject);
#endif

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
	ScoreText->SetColor({ 255, 0, 0, 1 });
	ScoreVisualsObject->AddComponent(ScoreText);
	const auto scoreUI = new ScoreUI(ScoreVisualsObject.get());
	ScoreVisualsObject->AddComponent(scoreUI);
	ScoreVisualsObject->SetLocalPosition({ 10, 550 });
	scene.Add(ScoreVisualsObject);

	//Add PepperUI
	auto PepperVisualsObject = std::make_shared<GameObject>();

	PepperVisualsObject->AddComponent(new RenderComponent(PepperVisualsObject.get()));
	const auto PepperText = new TextComponent(PepperVisualsObject.get());
	PepperText->SetColor({ 0, 255, 0, 1 });
	PepperVisualsObject->AddComponent(PepperText);
	const auto pepperUI = new PepperUI(PepperVisualsObject.get());
	PepperVisualsObject->AddComponent(pepperUI);
	PepperVisualsObject->SetLocalPosition({ Biggin::GetWindowWidth() - 125, 10 });
	scene.Add(PepperVisualsObject);

	//Add Player
	auto playerObject = std::make_shared<GameObject>();
	playerObject->Setname("Player");

	playerObject->AddComponent(new Subject(playerObject.get()));
	playerObject->AddComponent(new HealthComponent(playerObject.get(), 3, { healthUI }));
	playerObject->AddComponent(new ScoreComponent(playerObject.get(), { scoreUI, &CSteamAchievements::GetInstance() }));
	playerObject->AddComponent(new PepperShooter(playerObject.get(), { pepperUI }, 5));
	auto movementCompoent = new PossessGameObjectComponent(playerObject.get(), 75);
	playerObject->AddComponent(movementCompoent);
	auto peterPepper = new character::PeterPepper(playerObject.get());
	playerObject->AddComponent(peterPepper);
	b2Filter filterPlayer{};
	filterPlayer.maskBits = 0xFFFF ^ character::PeterPepper::PlayerCollisionGroup::playerIgnoreGroup; //Ignore group 4
	filterPlayer.categoryBits = character::PeterPepper::PlayerCollisionGroup::playerCollisionGroup; //set self to group 5
	playerObject->AddComponent(new BoxCollider2d(playerObject.get(), { 20, 30 }, false, b2_dynamicBody, {},
		"Player", {}, true, filterPlayer));
	playerObject->AddComponent(new RenderComponent(playerObject.get(), "BurgerTimeSpriteSheet.png"));
	auto playerSprite = new SpriteRenderComponent(playerObject.get(), { 9,{0,0},{32,32} }, 0.1f);
	playerSprite->AddAnimation(static_cast<int>(character::AnimationState::Idle), { 1, 1 });
	playerSprite->AddAnimation(static_cast<int>(character::AnimationState::RunHorizontal), { 3, 3 });
	playerSprite->AddAnimation(static_cast<int>(character::AnimationState::RunVertical), { 3, 6 });
	playerSprite->AddAnimation(static_cast<int>(character::AnimationState::PepperHorizontal), { 1, 10 });
	playerSprite->AddAnimation(static_cast<int>(character::AnimationState::PepperVertical), { 1, 11 });
	playerSprite->AddAnimation(static_cast<int>(character::AnimationState::Die), { 6, 12 });
	playerSprite->SetCurrentSprite(1);
	playerObject->AddComponent(playerSprite);

	scene.Add(playerObject);

	//Movement bindings
	InputManager::GetInstance().MapActionKey({ ActionState::Hold, ControllerButton::None, 0, SDLK_w },
		std::make_unique<MoveCommand>(movementCompoent, MoveCommand::ActionDirection::Up));
	InputManager::GetInstance().MapActionKey({ ActionState::Hold, ControllerButton::None, 0, SDLK_a },
		std::make_unique<MoveCommand>(movementCompoent, MoveCommand::ActionDirection::Left));
	InputManager::GetInstance().MapActionKey({ ActionState::Hold, ControllerButton::None, 0, SDLK_s },
		std::make_unique<MoveCommand>(movementCompoent, MoveCommand::ActionDirection::Down));
	InputManager::GetInstance().MapActionKey({ ActionState::Hold, ControllerButton::None, 0, SDLK_d },
		std::make_unique<MoveCommand>(movementCompoent, MoveCommand::ActionDirection::Right));

	InputManager::GetInstance().MapActionKey({ ActionState::ThumbL, ControllerButton::None, 0 },
		std::make_unique<MoveCommand>(movementCompoent, MoveCommand::ActionDirection::All));

	InputManager::GetInstance().MapActionKey({ ActionState::Down, ControllerButton::ButtonA, 0, SDLK_SPACE },
		std::make_unique<ShootCommand>(peterPepper));


	auto enemySpawner = std::make_shared<GameObject>();
	enemySpawner->Setname("EnemySpawner");
	enemySpawner->AddComponent(new enemy::EnemySpawner(enemySpawner.get(), true));
	scene.Add(enemySpawner);

	scene.Start();
}

void RunningState::Enter()
{
	SceneManager::GetInstance().SetScenesPaused(false);
	SoundServiceLocator::GetSoundSystem().UnpauseAll();
}

void RunningState::OnNotify(biggin::Component* /*entity*/, const std::string& event)
{
	if (event == "FinishedLevel")
	{

	}
	else if (event == "GameOver")
	{
		//save high-score
		//reset all
		//load main menu
		m_GameOver = true;
	}
}

void RunningState::RenderMenu(BurgerTimeMenuState*& currState)
{
	if (InputManager::GetInstance().IsPressed(SDLK_ESCAPE, ControllerButton::Start, 0)
		|| InputManager::GetInstance().IsPressed(SDLK_UNKNOWN, ControllerButton::Start, 1))
	{
		ChangeState(currState, m_pOptionsState);
	}

	if (m_GameOver)
	{
		auto& sceneManager = SceneManager::GetInstance();
		sceneManager.RemoveScene(sceneManager.GetActiveScene().GetName());
		ChangeState(currState, m_pHighScoreState);
		m_GameOver = false;
	}
}

void OptionsState::Enter()
{
	SceneManager::GetInstance().SetScenesPaused(true);
	SoundServiceLocator::GetSoundSystem().PauseAll();
}

void OptionsState::Exit()
{
	SceneManager::GetInstance().SetScenesPaused(false);
	SoundServiceLocator::GetSoundSystem().UnpauseAll();
}

void OptionsState::RenderMenu(BurgerTimeMenuState*& currState)
{
	if (InputManager::GetInstance().IsPressed(SDLK_ESCAPE ,ControllerButton::Start, 0)
		|| InputManager::GetInstance().IsPressed(SDLK_UNKNOWN, ControllerButton::Start, 1))
	{
		ChangeState(currState, m_pRunningState);
	}

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoBackground;
	window_flags |= ImGuiWindowFlags_NoTitleBar;
	window_flags |= ImGuiWindowFlags_NoResize;
	window_flags |= ImGuiWindowFlags_AlwaysAutoResize;
	//window_flags |= ImGuiWindowFlags_NavFlattened;

	ImGui::Begin("Options", &m_IsOpen, window_flags);
	if (ImGui::Button("MainMenu", { 200,50 }))
	{
		SoundServiceLocator::GetSoundSystem().Play("menu_select.wav", 0.2f);

		ChangeState(currState, m_pConfirmationState);
	}

	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();
	ImGui::Spacing();

	if (ImGui::Button("Resume", { 200,50 }))
	{
		SoundServiceLocator::GetSoundSystem().Play("menu_select.wav", 0.2f);

		ChangeState(currState, m_pRunningState);
	}

	ImGui::End();
}

void ConfirmationState::RenderMenu(BurgerTimeMenuState*& currState)
{
	ImGuiWindowFlags window_flags = 0;
	//window_flags |= ImGuiWindowFlags_NoBackground;
	window_flags |= ImGuiWindowFlags_NoTitleBar;
	window_flags |= ImGuiWindowFlags_NoResize;
	window_flags |= ImGuiWindowFlags_AlwaysAutoResize;
	//window_flags |= ImGuiWindowFlags_NavFlattened;

	ImGui::Begin("Confirmation", &m_IsOpen, window_flags);

	std::string text = "Are you sure you want to go to main menu?\nYou will lose all progress";
	auto windowWidth = ImGui::GetWindowSize().x;
	auto textWidth = ImGui::CalcTextSize(text.c_str()).x;

	ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
	ImGui::Text(text.c_str());

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	if (ImGui::Button("Yes", { 200,50 }))
	{
		SoundServiceLocator::GetSoundSystem().Play("menu_select.wav", 0.2f);

		auto& sceneManager = SceneManager::GetInstance();
		sceneManager.RemoveScene(sceneManager.GetActiveScene().GetName());

		ChangeState(currState, m_pMainMenuState);
	}

	ImGui::SameLine(250);

	if (ImGui::Button("No", { 200,50 }))
	{
		SoundServiceLocator::GetSoundSystem().Play("menu_select.wav", 0.2f);

		ChangeState(currState, m_pPrevState);
	}

	ImGui::End();
}

void ConfirmationState::Enter()
{
	SceneManager::GetInstance().SetScenesPaused(true);
	SoundServiceLocator::GetSoundSystem().PauseAll();
}

void ConfirmationState::Exit()
{
	SceneManager::GetInstance().SetScenesPaused(false);
	SoundServiceLocator::GetSoundSystem().UnpauseAll();
}

void HighScoreState::RenderMenu(BurgerTimeMenuState*& currState)
{
	m_ElapsedSec += GameTime::GetInstance().GetDeltaT();
	if (m_ElapsedSec > 5)
	{
		m_ElapsedSec = 0;
		ChangeState(currState, m_pMainMenuState);
	}
}

void HighScoreState::Enter()
{
	LoadHighScoreScene();
	SceneManager::GetInstance().ChangeActiveScene("HighScore");
}

void HighScoreState::Exit()
{
	auto& sceneManager = SceneManager::GetInstance();
	sceneManager.RemoveScene(sceneManager.GetActiveScene().GetName());
}

void HighScoreState::LoadHighScoreScene()
{
	auto& sceneManager = SceneManager::GetInstance();
	auto& scene = sceneManager.CreateScene("HighScore");

	//Add HighScoreUI
	auto ScoreVisualsObject = std::make_shared<GameObject>();

	ScoreVisualsObject->AddComponent(new RenderComponent(ScoreVisualsObject.get()));
	const auto ScoreText = new TextComponent(ScoreVisualsObject.get());
	ScoreText->SetColor({ 255, 0, 0, 1 });
	ScoreVisualsObject->AddComponent(ScoreText);
	const auto scoreUI = new HighScoreUI(ScoreVisualsObject.get());
	ScoreVisualsObject->AddComponent(scoreUI);
	ScoreVisualsObject->SetLocalPosition({ biggin::Biggin::GetWindowWidth()/2.f - 100, biggin::Biggin::GetWindowHeight() / 2.f - 300 });
	scene.Add(ScoreVisualsObject);

	//Add Score
	auto scoreObject = std::make_shared<GameObject>();

	scoreObject->AddComponent(new Subject(scoreObject.get()));
	scoreObject->AddComponent(new ScoreComponent(scoreObject.get(), { scoreUI }));
	scene.Add(scoreObject);

	scene.Start();

}
