#include "GameLoader.h"
#include "BurgerTimeMenuState.h"

using namespace biggin;

GameLoader::GameLoader()
	: m_pState(BurgerTimeMenuState::GetMainMenu())
{
	BurgerTimeMenuState::Start(m_pState);
}

GameLoader::~GameLoader()
{
	BurgerTimeMenuState::Cleanup();
}

void GameLoader::RenderMenu()
{
	m_pState->RenderMenu(m_pState);
}

