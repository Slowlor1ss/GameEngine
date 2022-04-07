#pragma once
#include <Box2D/Common/b2Math.h>
#include "Singleton.h"

class Box2dRenderer;
class b2Draw;
class b2World;

namespace biggin
{
	class ContactListener;

	class Box2dManager final : public Singleton<Box2dManager>
	{
	public:
		void Init(const b2Vec2& gravity = b2Vec2{0, 0}, int velocityIteration = 8, int positionIterations = 3);

		void Simulate() const;
		void RenderDebug() const;

		b2World* GetWorld() const { return m_pBox2dWorld; }

	private:
		friend class Singleton<Box2dManager>;
		Box2dManager() = default;

		b2World* m_pBox2dWorld = nullptr;
		Box2dRenderer* m_pDebugRenderer = nullptr; //TODO: fix memoryleaks

		ContactListener* m_ContactListener{nullptr};

		//settings
		int m_VelocityIterations{};
		int m_PositionIterations{};
	};
}

