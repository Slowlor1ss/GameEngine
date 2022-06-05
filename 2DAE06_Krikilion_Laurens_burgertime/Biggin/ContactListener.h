#pragma once
#pragma warning(push, 0)
#include <Box2D/Dynamics/b2WorldCallbacks.h>
#pragma warning (pop)

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

