#include <Biggin.h>
#include <iostream>
#include "GameLoader.h"
#include "MenuRenderer.h"
#include "SceneManager.h"

#define ADD_CONSOLE_IN_RELEASE

int main(int /*argc*/, char* /*argv*/[])
{
	biggin::SceneManager::GetInstance().CreateScene("MainMenu");

	biggin::Biggin engine;
	engine.Initialize();

	//biggin::InputManager::GetInstance().MapActionKey({ biggin::ActionState::Up, biggin::ControllerButton::Back, 0, SDLK_c },
	//	std::make_unique<PrintControls>());

	biggin::MenuRenderer::GetInstance().SetMenu(new GameLoader());
	engine.Run();

	return 0;
}
