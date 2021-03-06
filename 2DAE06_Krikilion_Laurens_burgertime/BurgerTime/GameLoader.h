#pragma once
#include "MenuState.h"

namespace biggin
{
	class Observer;
}

class BurgerTimeMenuState;
enum class BurgerIngredients;

class GameLoader final : public biggin::MenuState
{
public:
	GameLoader();
	~GameLoader() override;

	GameLoader(const GameLoader& other) = delete;
	GameLoader(GameLoader&& other) noexcept = delete;
	GameLoader& operator=(const GameLoader& other) = delete;
	GameLoader& operator=(GameLoader&& other) noexcept = delete;

	void RenderMenu() override;
	BurgerTimeMenuState* GetState() const { return m_pState; }
	void SetState(BurgerTimeMenuState* state) { m_pState = state; }
	//void LoadGame();

private:

	BurgerTimeMenuState* m_pState;

};

