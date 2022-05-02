#pragma once
#include <glm/detail/type_vec.hpp>
#include "Component.h"

enum class BurgerIngredients;

class GameLoader final : public biggin::Component
{
public:
	GameLoader(biggin::GameObject* go);
	~GameLoader() = default;

	GameLoader(const GameLoader& other) = delete;
	GameLoader(GameLoader&& other) noexcept = delete;
	GameLoader& operator=(const GameLoader& other) = delete;
	GameLoader& operator=(GameLoader&& other) noexcept = delete;

	void RenderUi() override;
	void LoadGame();

	static void BurgerPrefab(BurgerIngredients ingredient, glm::vec2 pos);

private:


	bool m_IsOpen{true};

};

