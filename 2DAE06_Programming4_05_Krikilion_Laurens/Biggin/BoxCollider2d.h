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
		~BoxCollider2d();

		void FixedUpdate() override;

		void BeginContact(const BoxCollider2d* other, b2Contact* contact);
		void EndContact(const BoxCollider2d* other);

	private:
		b2Body* m_pBody = nullptr;
		b2Fixture* m_Fixture;
		//b2PolygonShape* b2Box;//todo:Fix this either use it or remove it if use add m_
		Subject* m_pNotifier;
		GameObject* m_GameObjectRef;
		b2Joint* joint;
		b2Body* b2;
		bool m_IsColliding{false};
	};
}

