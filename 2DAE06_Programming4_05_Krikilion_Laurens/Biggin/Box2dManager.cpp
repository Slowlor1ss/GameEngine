#include "BigginPCH.h"
#include "Box2dManager.h"
#include <Box2D/Common/b2Draw.h>
#include <Box2D/Common/b2Math.h>
#include <Box2D/Dynamics/b2World.h>
#include "ContactListener.h"
#include "Box2dRenderer.h"
#include "GameTime.h"

using namespace biggin;

void Box2dManager::Init(const b2Vec2& gravity, int velocityIteration, int positionIterations)
{
	m_VelocityIterations = velocityIteration;
	m_PositionIterations = positionIterations;


	//Create Physics;
	m_pBox2dWorld = new b2World(gravity);
	m_pBox2dWorld->SetAllowSleeping(true);
	m_pBox2dWorld->SetWarmStarting(true);
	m_pBox2dWorld->SetContinuousPhysics(true);
	m_pBox2dWorld->SetSubStepping(false);

	m_pDebugRenderer = new Box2dRenderer();
	m_pDebugRenderer->SetFlags(b2Draw::e_shapeBit);
	m_pBox2dWorld->SetDebugDraw(m_pDebugRenderer);


	//create contact listener to allow overlap events
	m_ContactListener = new ContactListener(m_pBox2dWorld);
}

void Box2dManager::Simulate() const
{
	if (!m_pBox2dWorld)
		return;

	m_pBox2dWorld->Step(GameTime::GetFixedTimeStep(), m_VelocityIterations, m_PositionIterations);
}

void Box2dManager::RenderDebug() const
{
	if (m_pDebugRenderer != nullptr)
		m_pBox2dWorld->DrawDebugData();
}
