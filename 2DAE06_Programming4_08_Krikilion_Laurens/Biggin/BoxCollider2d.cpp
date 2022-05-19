#include "BigginPCH.h"
#include "BoxCollider2d.h"
#include <Box2D/Collision/Shapes/b2PolygonShape.h>
#include <Box2D/Dynamics/b2Fixture.h>
#include <Box2D/Dynamics/b2World.h>
#include "Box2dManager.h"
#include "GameObject.h"
#include "GameTime.h"
#include "Logger.h"
#include "Scene.h"
#include "SceneManager.h"
#include "Subject.h"
#include "Box2dManager.h"

using namespace biggin;

BoxCollider2d::BoxCollider2d(GameObject* go, const glm::vec2& size, bool isTrigger, b2BodyType CollisionType, const std::vector<Observer*>& observers,
							const std::string& tag, const glm::vec2& localOffset, bool centered, const b2Filter& filter) : Component(go)
	, m_pNotifier(go->GetComponent<Subject>())
	, m_GameObjectRef(go)
	, m_LocalOffset(localOffset.x, localOffset.y)
	, m_Tag(tag)
{
	if (m_pNotifier == nullptr)
		Logger::GetInstance().LogErrorAndBreak("Missing Subject Component");

	for (const auto observer : observers)
		m_pNotifier->AddObserver(observer);

	//m_B2WorldRef = biggin::SceneManager::GetInstance().GetActiveScene().Getb2World();
	m_Box2dManagerRef = biggin::SceneManager::GetInstance().GetActiveScene().Getb2Manager();
	const glm::vec2& pos = go->GetWorldPosition();

	//define the body
	b2BodyDef bd;
	bd.position.Set(pos.x + m_LocalOffset.x, pos.y + m_LocalOffset.y);
	bd.allowSleep = false;
	bd.type = CollisionType;
	bd.linearDamping = 7.f;

	//creation of the body
	if (m_Box2dManagerRef != nullptr)
		m_pBody = m_Box2dManagerRef->GetWorld()->CreateBody(&bd);

	//creating the shape
	b2PolygonShape b2Box;
	if (centered)
		//aligns center left with given pos
		b2Box.SetAsBox(size.x / 2.0f, size.y / 2.0f, b2Vec2(0.0f, 0.0f), 0);
	else
		//aligns bottom left with given pos
		b2Box.SetAsBox(size.x / 2.0f, size.y / 2.0f, b2Vec2(size.x / 2.0f, size.y / 2.0f), 0);

	//creating the fixture
	b2FixtureDef fd;
	fd.shape = &b2Box;
	fd.isSensor = isTrigger;
	fd.userData = this;
	fd.density = 0.f;
	fd.friction = 0.f;
	fd.filter = filter;
	const auto fixture = m_pBody->CreateFixture(&fd);
	fixture->SetUserData(this);

}

BoxCollider2d::~BoxCollider2d()
{
	if (m_Box2dManagerRef != nullptr && m_pBody != nullptr)
	{
		m_pBody->SetUserData(nullptr);
		m_Box2dManagerRef->GetWorld()->DestroyBody(m_pBody);
		m_pBody = nullptr;
	}
}

void biggin::BoxCollider2d::Start()
{
	const glm::vec2 pos{ m_GameObjectRef->GetWorldPosition() };
	m_pBody->SetTransform({ pos.x + m_LocalOffset.x ,pos.y + m_LocalOffset.y }, 0);
}

void BoxCollider2d::FixedUpdate()
{
	m_EnableCollider = true;

	const glm::vec2 pos{ m_GameObjectRef->GetWorldPosition() };
	const auto transform = b2Vec2{ pos.x, pos.y } + m_LocalOffset - m_pBody->GetPosition() ;

	if (transform == b2Vec2_zero)
		return;

	m_pBody->SetTransform(b2Vec2{ pos.x, pos.y } + m_LocalOffset, 0);
	//manually preform a step with high position iterations to prevent tunneling
	m_Box2dManagerRef->GetWorld()->Step(GameTime::GetFixedTimeStep(), 8, 10);
	//m_pBody->ApplyLinearImpulseToCenter(10.f * transform, true);
	auto wp = m_GameObjectRef->GetWorldPosition();
	const auto posb = m_pBody->GetPosition() - m_LocalOffset - b2Vec2{ wp.x, wp.y };
	m_GameObjectRef->TranslateLocalPosition({posb.x, posb.y});
	//m_GameObjectRef->SetLocalPosition(posb.x, posb.y);
}

void BoxCollider2d::BeginContact(BoxCollider2d* other)
{
	//ignores first frame ovelap events when some object haven't been positioned yet
	if (!m_EnableCollider) return;

	other->m_pOtherCollider = this;
	m_pNotifier->notify(other, "BeginContact");
}

void BoxCollider2d::EndContact(BoxCollider2d* other)
{
	//ignores first frame ovelap events when some object haven't been positioned yet
	if (!m_EnableCollider) return;

	m_pNotifier->notify(other, "EndContact");
	other->m_pOtherCollider = nullptr;
}
