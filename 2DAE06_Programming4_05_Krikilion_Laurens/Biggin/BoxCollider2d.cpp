#include "BigginPCH.h"
#include "BoxCollider2d.h"
#include <Box2D/Collision/Shapes/b2PolygonShape.h>
#include <Box2D/Dynamics/b2Fixture.h>
#include <Box2D/Dynamics/b2World.h>
#include <Box2D/Dynamics/Contacts/b2Contact.h>
#include <Box2D/Dynamics/Joints/b2MouseJoint.h>
#include "Box2dManager.h"
#include "GameObject.h"
#include "Logger.h"
#include "Subject.h"

using namespace biggin;

BoxCollider2d::BoxCollider2d(biggin::GameObject* go, const glm::vec2& size, bool isTrigger, const std::vector<Observer*>& observers, b2BodyType CollisionType) : Component(go)
	, m_pNotifier(go->GetComponent<Subject>())
	, m_GameObjectRef(GetGameObject())
{
	if (m_pNotifier == nullptr)
		Logger::GetInstance().LogErrorAndBreak("Missing Subject Component");

	for (const auto observer : observers)
		m_pNotifier->AddObserver(observer);

	const Box2dManager& box2dManager = Box2dManager::GetInstance();
	const glm::vec2& pos = go->GetLocalPosition();

	//define the body
	b2BodyDef bd;
	bd.position.Set(pos.x, pos.y);
	bd.allowSleep =  false;
	bd.type = CollisionType;
	bd.linearDamping = 7.f;

	//creation of the body
	if (box2dManager.GetWorld() != nullptr)
		m_pBody = box2dManager.GetWorld()->CreateBody(&bd);

	//creating the shape
	b2PolygonShape* b2Box = new b2PolygonShape;
	b2Box->SetAsBox(size.x / 2.0f, size.y / 2.0f, b2Vec2(0.0f, 0.0f), 0);

	//creating the fixture
	b2FixtureDef fd;
	fd.shape = b2Box;
	fd.isSensor = isTrigger;
	fd.userData = this;
	fd.density = 0.f;
	fd.friction = 0.f;
	m_Fixture = m_pBody->CreateFixture(&fd);
	m_Fixture->SetUserData(this);

	//b2BodyDef groundBodyDef{};
	//groundBodyDef.position = {10,10};

	//b2 = box2dManager.GetWorld()->CreateBody(&bd);
	//b2->CreateFixture(&fd);

	//b2MouseJointDef mjd{};
	//mjd.bodyA = m_pBody;
	//mjd.bodyB = b2;
	////mjd.userData = this;
	////mjd.dampingRatio = 1.f;
	////mjd.frequencyHz = 60;
	//mjd.maxForce = 99999999.f;
	//mjd.collideConnected = true;
	//joint = box2dManager.GetWorld()->CreateJoint(&mjd);
}

biggin::BoxCollider2d::~BoxCollider2d()
{
	//delete m_Fixture;
	//m_Fixture = nullptr;
}

void BoxCollider2d::FixedUpdate()
{
	//if (!m_IsColliding)
	//{
		const glm::vec2 pos{ m_GameObjectRef->GetLocalPosition() };
		auto transform = b2Vec2{pos.x, pos.y} - m_pBody->GetPosition();
		//m_pBody->SetTransform({pos.x, pos.y}, 0);
		m_pBody->ApplyLinearImpulseToCenter(10.f * transform, true);
		//b2Box->m_centroid = { pos.x, pos.y };
		//m_pBody->SetLinearVelocity({ 0, 10 });
		//static_cast<b2MouseJoint*>(joint)->SetTarget({ pos.x,pos.y });
	//}
	//else
	//{
		auto posb = m_pBody->GetPosition();
		m_GameObjectRef->SetLocalPosition(posb.x, posb.y);
	//}
}

void BoxCollider2d::BeginContact(const BoxCollider2d* other, b2Contact* /*contact*/)
{
	m_pNotifier->notify(other, "BeginContact");
	auto pos = m_pBody->GetPosition();
	m_GameObjectRef->SetLocalPosition(pos.x, pos.y);
	m_IsColliding = true;
}

void BoxCollider2d::EndContact(const BoxCollider2d* other)
{
	m_pNotifier->notify(other, "EndContact");
	m_IsColliding = false;
}
