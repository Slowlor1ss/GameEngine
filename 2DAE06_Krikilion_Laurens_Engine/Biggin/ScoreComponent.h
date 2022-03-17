#pragma once
#include "Component.h"

namespace biggin
{
	class Subject;

	class ScoreComponent : public Component
	{
	public:
		ScoreComponent(GameObject* go);

		void Start() override;

		int GetScore() const;
		void IncreaseScore(int amount);

	private:
		int m_Score;

		Subject* m_pNotifier;
	};
}

