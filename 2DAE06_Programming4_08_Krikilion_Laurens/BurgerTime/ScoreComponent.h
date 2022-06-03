#pragma once
#include "Component.h"

namespace biggin
{
	class Observer;
	class Subject;
}

namespace burgerTime
{
	class ScoreComponent : public biggin::Component
	{
	public:
		ScoreComponent(biggin::GameObject* go, const std::vector<biggin::Observer*>& observers = {});

		void Start() override;

		int GetScore() const;
		void IncreaseScore(int amount);

	private:
		int m_Score{};

		biggin::Subject* m_pNotifier;
	};

	enum ScoreValues
	{
		burgerDropped = 50,
		hotDogDied = 100,
		pickleDied = 200,
		eggDied = 300,
		burgerDroppedWithEnemy = 500,
	};

}