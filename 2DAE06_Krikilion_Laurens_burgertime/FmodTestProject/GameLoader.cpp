#include "GameLoader.h"
#include <RenderComponent.h>
#include <Box2D/Dynamics/b2Fixture.h>
#include "BoxCollider2d.h"
#include "Commands.hpp"
#include "FModSoundSystem.h"
#include "GameObject.h"
#include "InputManager.h"
#include "PeterPepper.h"
#include "PossessGameObjectComponent.h"
#include "Scene.h"
#include "SceneManager.h"
#include "SpriteRenderComponent.h"
#include "GameTime.h"

using namespace biggin;

FmodSoundSystem fmodSound{};
std::string path = R"(D:\Personal\Audio\GameEngine\2DAE06_Krikilion_Laurens_burgertime\Data\Sounds\)";
auto& gameTime = GameTime::GetInstance();
idx id;
auto playerObject = std::make_shared<GameObject>();

GameLoader::GameLoader()
{
	FmodSoundSystem::Init();
	FmodSoundSystem::SoundDefinition sd = { path + "main.wav" , 1, 0, 50, true, true};
	id = fmodSound.RegisterSound(sd, true);
	fmodSound.PlaySound(id, { 100,100 }, 1);
	
	//fmodSound.LoadSound(path + "burger_going_down.wav", false);
	//fmodSound.LoadSound(path + "main.wav", true);
	//do
	//{
	//} while (!fmodSound.IsSoundReady(path + "main.wav"));
	//fmodSound.PlaySound(path+"burger_going_down.wav", {0,0}, FmodSoundSystem::VolumeTodB(1.0f));
	//fmodSound.PlaySound(path+"main.wav", {0,0}, FmodSoundSystem::VolumeTodB(1.0f));


#pragma region EngineStuff
	auto& sceneManager = SceneManager::GetInstance();
	auto& scene = sceneManager.GetActiveScene();

	auto audioObject = std::make_shared<GameObject>();
	auto audioRender = new RenderComponent(audioObject.get(), "icon.png");
	audioObject->AddComponent(audioRender);
	audioRender->SetOffset({ 100, 100 });

	scene.Add(audioObject);

	//Add Player
	//auto playerObject = std::make_shared<GameObject>();
	playerObject->Setname("Player");

	playerObject->AddComponent(new Subject(playerObject.get()));
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

	//InputManager::GetInstance().MapActionKey({ ActionState::Down, ControllerButton::ButtonA, 0, SDLK_SPACE },
	//	std::make_unique<PlayAudioCommand>(peterPepper));
#pragma endregion
}

GameLoader::~GameLoader()
{
	fmodSound.UnloadSound(id);
	FmodSoundSystem::Shutdown();
}

void GameLoader::RenderMenu()
{
	FmodSoundSystem::Update(gameTime.GetDeltaT(), playerObject.get()->GetWorldPosition());
}

