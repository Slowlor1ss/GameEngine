#include "BigginPCH.h"
#include "ContactListener.h"
#pragma warning(push, 0)
#include <Box2D/Dynamics/b2World.h>
#include <Box2D/Dynamics/Contacts/b2Contact.h>
#pragma warning (pop)
#include "BoxCollider2d.h"

using namespace biggin;

biggin::ContactListener::ContactListener(b2World* world)
{
	world->SetContactListener(this);
}

void ContactListener::BeginContact(b2Contact* contact)
{
	auto* colliderA = static_cast<BoxCollider2d*>(contact->GetFixtureA()->GetUserData());
	auto* colliderB = static_cast<BoxCollider2d*>(contact->GetFixtureB()->GetUserData());

	colliderA->BeginContact(colliderB);
	colliderB->BeginContact(colliderA);
}

void ContactListener::EndContact(b2Contact* contact)
{
	auto* colliderA = static_cast<BoxCollider2d*>(contact->GetFixtureA()->GetUserData());
	auto* colliderB = static_cast<BoxCollider2d*>(contact->GetFixtureB()->GetUserData());

	colliderA->EndContact(colliderB);
	colliderB->EndContact(colliderA);
}
