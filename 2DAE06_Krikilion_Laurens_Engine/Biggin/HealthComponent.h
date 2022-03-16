#pragma once
#include "Component.h"

namespace biggin
{
	class HealthComponent final : public biggin::Component
	{
	public:
		HealthComponent(biggin::GameObject* go, int initialLives = 4);

		void Start() override {};
		void Update() override {};
		void FixedUpdate() override {};
		void Render() const override {};
		void RenderUi() override {};

		int GetLives() const;

	private:
		int m_LivesLeft;

		SubjectComponent* m_pBroadcasterRef;
	};
}

