#include "BigginPCH.h"
#include "BoxCollider2d.h"
#include <Box2D/Collision/Shapes/b2PolygonShape.h>
#include <Box2D/Dynamics/b2Fixture.h>
#include <Box2D/Dynamics/b2World.h>
#include "Box2dManager.h"
#include "GameObject.h"
#include "Subject.h"

using namespace biggin;

BoxCollider2d::BoxCollider2d(biggin::GameObject* go, const glm::vec2& size, bool isTrigger, const std::vector<Observer*>& observers, b2BodyType CollisionType) : Component(go)
	, m_pNotifier(go->GetComponent<Subject>())
	, m_GameObjectRef(GetGameObject())
{
	for (const auto observer : observers)
		m_pNotifier->AddObserver(observer);

	const Box2dManager& box2dManager = Box2dManager::GetInstance();
	const glm::vec2& pos = go->GetLocalPosition();

	//define the body
	b2BodyDef bd;
	bd.position.Set(pos.x, pos.y);
	bd.allowSleep =  false;
	bd.type = CollisionType;

	//creation of the body
	if (box2dManager.GetWorld() != nullptr)
		m_pBody = box2dManager.GetWorld()->CreateBody(&bd);

	//creating the shape
	b2PolygonShape b2Box;
	b2Box.SetAsBox(size.x / 2.0f, size.y / 2.0f, b2Vec2(0.0f, 0.0f), 0);

	//creating the fixture
	b2FixtureDef fd;
	fd.shape = &b2Box;
	fd.isSensor = isTrigger;
	fd.userData = this;
	m_Fixture = m_pBody->CreateFixture(&fd);
	m_Fixture->SetUserData(this);
}

void biggin::BoxCollider2d::FixedUpdate()
{
	const glm::vec2 pos{ m_GameObjectRef->GetLocalPosition() };
	m_pBody->SetTransform({pos.x, pos.y}, 0);
}

void BoxCollider2d::BeginContact(const BoxCollider2d* other) const
{
	m_pNotifier->notify(other, "BeginContact");
}

void BoxCollider2d::EndContact(const BoxCollider2d* other) const
{
	m_pNotifier->notify(other, "EndContact");
}
