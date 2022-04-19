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

BoxCollider2d::BoxCollider2d(GameObject* go, const glm::vec2& size, bool isTrigger, b2BodyType CollisionType, const std::vector<Observer*>& observers,
							std::string tag, const glm::vec2& localOffset, bool centered) : Component(go)
	, m_pNotifier(go->GetComponent<Subject>())
	, m_GameObjectRef(go)
	, m_LocalOffset(localOffset.x, localOffset.y)
	, m_Tag(tag)
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
	b2PolygonShape b2Box;
	if (centered)
		//aligns center left with given pos
		b2Box.SetAsBox(size.x / 2.0f, size.y / 2.0f, b2Vec2(0.0f, 0.0f), 0);
	else
		//aligns bottom left with given pos
		b2Box.SetAsBox(size.x / 2.0f, size.y / 2.0f, b2Vec2(size.x / 2.0f, -size.y / 2.0f), 0);

	//creating the fixture
	b2FixtureDef fd;
	fd.shape = &b2Box;
	fd.isSensor = isTrigger;
	fd.userData = this;
	fd.density = 0.f;
	fd.friction = 0.f;
	auto fixture = m_pBody->CreateFixture(&fd);
	fixture->SetUserData(this);
}

BoxCollider2d::~BoxCollider2d()
{
	const auto& b2World = Box2dManager::GetInstance().GetWorld();
	if (b2World != nullptr && m_pBody != nullptr)
	{
		m_pBody->SetUserData(nullptr);
		b2World->DestroyBody(m_pBody);
		m_pBody = nullptr;
	}
}

void biggin::BoxCollider2d::Start()
{
	const glm::vec2 pos{ m_GameObjectRef->GetLocalPosition() };
	m_pBody->SetTransform({ pos.x + m_LocalOffset.x ,pos.y + m_LocalOffset.y }, 0);
}

void BoxCollider2d::FixedUpdate()
{
	//TODO: maybe only update this if the object actually moved
	//ugly way of doing things but I couldn't find a better way to move the character and keep using collisions for now
	const glm::vec2 pos{ m_GameObjectRef->GetLocalPosition() };
	auto transform = b2Vec2{pos.x, pos.y} - m_pBody->GetPosition() + m_LocalOffset;

	if (transform == b2Vec2_zero)
		return;

	m_pBody->ApplyLinearImpulseToCenter(10.f * transform, true);

	auto posb = m_pBody->GetPosition();
	m_GameObjectRef->SetLocalPosition(posb.x, posb.y);
}

void BoxCollider2d::BeginContact(const BoxCollider2d* other)
{
	m_pNotifier->notify(other, "BeginContact");
	m_IsColliding = true;
}

void BoxCollider2d::EndContact(const BoxCollider2d* other)
{
	m_pNotifier->notify(other, "EndContact");
	m_IsColliding = false;
}
