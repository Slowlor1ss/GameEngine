#pragma once
#include "BoxCollider2d.h"
#include "Component.h"
#include "Observer.h"
#include "ScoreComponent.h"


namespace biggin
{
	class GameTime;
	class Subject;
	class BoxCollider2d;
}

enum class BurgerIngredients
{
	BurgerTop,
	Lettuce,
	Meat,
	BurgerBottom,
	Cheese,
	Tomato,
};

class Burger final : public biggin::Component, public biggin::Observer
{
public:
	Burger(biggin::GameObject* go, BurgerIngredients ingredient, const std::vector<Observer*>& observers);
	~Burger() override;

	Burger(const Burger& other) = delete;
	Burger(Burger&& other) noexcept = delete;
	Burger& operator=(const Burger& other) = delete;
	Burger& operator=(Burger&& other) noexcept = delete;

	void Initialize(biggin::GameObject*) override;
	void Start() override;
	void OnNotify(Component* entity, const std::string& event) override;
	void FixedUpdate() override;
	void Update() override;

	void InitializeBurger();

	bool ReachedBottom() const { return m_ReachedBottom; }
	bool IsFalling() const { return m_IsFalling; }
	void IncreaseEnemyOnBurgerCounter() { ++m_EnemiesOnBurger; }
	void DecreaseEnemyOnBurgerCounter() { --m_EnemiesOnBurger; }
	const biggin::GameObject* GetBurgerResponsible() const { return m_pPlayerGo; } //game object responsible for dropping the burger
	static constexpr int GetBurgerSize() { return m_BurgerSize; }

private:
	void InitRenderComp(int collumnIdx) const;
	void DropBurger(int score = burgerTime::ScoreValues::burgerDropped);

	bool m_Initialized{ false };

	int m_AmntTouchedParts{0};
	bool m_IsFalling{ false };
	bool m_ReachedBottom{ false };
	float m_Velocity{ 100.f };
	//std::vector<biggin::RenderComponent*> m_RenderComponents;
	std::vector<biggin::GameObject*> m_pChilderen;
	//std::vector<biggin::BoxCollider2d*> m_ColliderComponents;
	static constexpr int m_BurgerSize = 4;
	bool m_Touched[m_BurgerSize];

	biggin::Subject* m_pNotifier{ nullptr };

	BurgerIngredients m_Ingredient;

	int m_EnemiesOnBurger{};
	biggin::GameTime& m_GameTimeRef;

	utils::DelayedCallback m_FallDelayed{};
	const biggin::GameObject* m_pPlayerGo{nullptr};

public:
	enum BurgerCollisionGroup : unsigned short
	{
		burgerIgnoreGroup = biggin::BoxCollider2d::CollisionGroup::Group1,
		burgerCollisionGroup = biggin::BoxCollider2d::CollisionGroup::Group2,
	};
};

