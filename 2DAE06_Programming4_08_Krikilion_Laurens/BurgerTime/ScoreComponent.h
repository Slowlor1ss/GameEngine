#pragma once
#include "Component.h"

namespace biggin
{
	class Observer;
	class Subject;

	class ScoreComponent : public Component
	{
	public:
		ScoreComponent(GameObject* go, const std::vector<Observer*>& observers = {});

		void Start() override;

		int GetScore() const;
		void IncreaseScore(int amount);

	private:
		int m_Score;

		Subject* m_pNotifier;
	};
}

