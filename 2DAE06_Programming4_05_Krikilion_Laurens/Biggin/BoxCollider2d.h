#pragma once
#include <Box2D/Dynamics/b2Body.h>
#include "Component.h"

namespace biggin
{
	class Subject;
	class Observer;

	class BoxCollider2d final : public Component
	{
	public:
		explicit BoxCollider2d(GameObject* go, const glm::vec2& size, bool isTrigger, const std::vector<biggin::Observer*>& observers = {}, b2BodyType CollisionType = { b2_dynamicBody });

		void FixedUpdate() override;

		void BeginContact(const BoxCollider2d* other) const;
		void EndContact(const BoxCollider2d* other) const;

	private:
		b2Body* m_pBody = nullptr;
		b2Fixture* m_Fixture;
		Subject* m_pNotifier;
		GameObject* m_GameObjectRef;
	};
}

