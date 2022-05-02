#pragma once
#include "Component.h"
#include "Observer.h"

enum class BurgerIngredients
{
	BurgerTop,
	Lettuce,
	Meat,
	BurgerBottom,
};

class Burger final : public biggin::Component, public biggin::Observer
{
public:
	Burger(biggin::GameObject* go, glm::vec2 pos, float tileSize, BurgerIngredients ingredient);
	~Burger() override = default;

	Burger(const Burger& other) = delete;
	Burger(Burger&& other) noexcept = delete;
	Burger& operator=(const Burger& other) = delete;
	Burger& operator=(Burger&& other) noexcept = delete;

	void OnNotify(const Component* entity, const std::string& event) override;
	void FixedUpdate() override;

	bool ReachedBottom() const { return m_ReachedBottom; }
	static int GetBurgerSize() { return m_BurgerSize; }

private:
	int m_AmntTouchedParts{0};
	bool m_IsFalling{ false };
	bool m_ReachedBottom{ false };
	float m_Velocity{ 9.81f };
	static constexpr int m_BurgerSize = 6;
};

