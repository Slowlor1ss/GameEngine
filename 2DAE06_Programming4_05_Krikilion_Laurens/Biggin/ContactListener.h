#pragma once
#include <Box2D/Dynamics/b2WorldCallbacks.h>

class b2World;

namespace biggin
{
	class ContactListener final : public b2ContactListener
	{
	public:
		ContactListener(b2World* world);

		void BeginContact(b2Contact* contact) override;
		void EndContact(b2Contact* contact) override;
	};
}

