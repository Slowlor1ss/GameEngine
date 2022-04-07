#include "BigginPCH.h"
#include "ContactListener.h"
#include <Box2D/Dynamics/b2World.h>
#include <Box2D/Dynamics/Contacts/b2Contact.h>
#include "Box2dManager.h"
#include "BoxCollider2d.h"

using namespace biggin;

biggin::ContactListener::ContactListener(b2World* world)
{
	world->SetContactListener(this);
}

void ContactListener::BeginContact(b2Contact* contact)
{
	const auto* colliderA = static_cast<BoxCollider2d*>(contact->GetFixtureA()->GetUserData());
	const auto* colliderB = static_cast<BoxCollider2d*>(contact->GetFixtureB()->GetUserData());

	colliderA->BeginContact(colliderB);
	colliderA->BeginContact(colliderA);
}

void ContactListener::EndContact(b2Contact* contact)
{
	const auto* colliderA = static_cast<BoxCollider2d*>(contact->GetFixtureA()->GetUserData());
	const auto* colliderB = static_cast<BoxCollider2d*>(contact->GetFixtureB()->GetUserData());

	colliderA->EndContact(colliderB);
	colliderA->EndContact(colliderA);
}
