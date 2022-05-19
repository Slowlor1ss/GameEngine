#pragma once
#include "Component.h"
#include "Observer.h"
#include "RenderComponent.h"

namespace biggin
{
	class BoxCollider2d;
}

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
	Burger(biggin::GameObject* go, BurgerIngredients ingredient);
	~Burger() override = default;

	Burger(const Burger& other) = delete;
	Burger(Burger&& other) noexcept = delete;
	Burger& operator=(const Burger& other) = delete;
	Burger& operator=(Burger&& other) noexcept = delete;

	void Start() override;
	void OnNotify(Component* entity, const std::string& event) override;
	void FixedUpdate() override;

	bool ReachedBottom() const { return m_ReachedBottom; }
	static int GetBurgerSize() { return m_BurgerSize; }

private:
	void InitRenderComp(int collumnIdx) const;

	int m_AmntTouchedParts{0};
	bool m_IsFalling{ false };
	bool m_ReachedBottom{ false };
	float m_Velocity{ 100.f };
	//std::vector<biggin::RenderComponent*> m_RenderComponents;
	std::vector<biggin::GameObject*> m_Childeren;
	//std::vector<biggin::BoxCollider2d*> m_ColliderComponents;
	static constexpr int m_BurgerSize = 4;
	bool m_Touched[m_BurgerSize];

	BurgerIngredients m_Ingredient;
};

