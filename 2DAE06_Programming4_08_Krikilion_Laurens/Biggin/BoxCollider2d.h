#pragma once
#include <Box2D/Dynamics/b2Body.h>
#include "Component.h"
#pragma warning(push)
#pragma warning (disable:4201)
#include <Box2D/Dynamics/b2Fixture.h>
#include <glm/glm.hpp>
#include "Scene.h"
#include "Subject.h"
#pragma warning(pop)

namespace biggin
{
	class Subject;
	class Observer;

	class BoxCollider2d final : public Component
	{
	public:
		explicit BoxCollider2d(GameObject* go, const glm::vec2& size, bool isTrigger,
		                       b2BodyType CollisionType = {b2_dynamicBody}, const std::vector<Observer*>& observers = {},
		                       const std::string& tag = {}, const glm::vec2& LocalOffset = {}, bool centered = true, const b2Filter& filter = {});
		~BoxCollider2d() override;

		void Start() override;
		void FixedUpdate() override;

		void BeginContact(BoxCollider2d* other);
		void EndContact(BoxCollider2d* other);

		void AddObservers(const std::vector<Observer*>& observers) const;

		std::string GetTag() const { return m_Tag; }
		void SetTag(const std::string& tag) { m_Tag = tag ; }

		//Get the other collider were currently overlapping with
		BoxCollider2d* GetOther() const { return m_pOtherCollider; }

		glm::vec2 GetLocalPos() { return { m_LocalOffset.x,m_LocalOffset.y }; }
		void TranslateLocalOffset(const glm::vec2& transform) { m_LocalOffset += b2Vec2{ transform.x,transform.y }; }

		GameObject* GetOwningGameObject() const { return GetGameObject(); }

	private:
		b2Body* m_pBody{ nullptr };

		Subject* m_pNotifier{ nullptr };
		GameObject* m_GameObjectRef{ nullptr };

		b2Vec2 m_LocalOffset;
		std::string m_Tag;

		bool m_EnableCollider{ false };

		//collider were currently overlapping with
		BoxCollider2d* m_pOtherCollider{nullptr};
		std::shared_ptr<Box2dManager> m_Box2dManagerRef;
	};

	enum class CollisionGroup : unsigned short
	{
		None = (1 << 0),
		Group1 = (1 << 1),
		Group2 = (1 << 2),
		Group3 = (1 << 3),
		Group4 = (1 << 4),
		Group5 = (1 << 5),
		Group6 = (1 << 6),
		Group7 = (1 << 7),
		Group8 = (1 << 8),
		Group9 = (1 << 9)
	};
}

