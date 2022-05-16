#pragma once
#include <glm/detail/type_vec.hpp>
#include "Component.h"
#include "MenuState.h"

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

	static void BurgerPrefab(BurgerIngredients ingredient, glm::vec2 pos);

private:

	BurgerTimeMenuState* m_pState;

};

