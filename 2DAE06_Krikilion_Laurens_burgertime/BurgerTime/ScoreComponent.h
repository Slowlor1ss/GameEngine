#pragma once
#include <set>
#include "Component.h"
#include <string>

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

		void SaveHighScore();
		std::multiset<int, std::greater<int>> GetHighScores() const;

	private:
		int m_Score{};
		const std::string m_HighScoreFilePath;

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