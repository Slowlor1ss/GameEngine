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
		explicit BoxCollider2d(GameObject* go, const glm::vec2& size, bool isTrigger,
		                       b2BodyType CollisionType = {b2_dynamicBody}, const std::vector<Observer*>& observers = {},
		                       std::string tag = {}, const glm::vec2& LocalOffset = {}, bool centered = true);
		~BoxCollider2d() override;

		void Start() override;
		void FixedUpdate() override;

		void BeginContact(const BoxCollider2d* other);
		void EndContact(const BoxCollider2d* other);

		std::string GetTag() const { return m_Tag; }

	private:
		b2Body* m_pBody{ nullptr };

		Subject* m_pNotifier{ nullptr };
		GameObject* m_GameObjectRef{ nullptr };

		b2Vec2 m_LocalOffset;
		std::string m_Tag;

		bool m_IsColliding{false};
	};
}

