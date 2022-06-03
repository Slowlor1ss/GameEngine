#pragma once
#include "Subject.h"

namespace biggin
{
	class Component;

	class Observer
	{
	public:
		virtual ~Observer()
		{
			if (!m_pNotifiers.empty())
				for (const auto pNotifier : m_pNotifiers)
				{
					pNotifier->RemoveObserver(this);
				}
		}
		virtual void OnNotify(Component* entity, const std::string& event) = 0;

	private:
		//pretty ugly very tightly coupled
		//I would like to not do this but as i don't have enough time
		//to change my collision system to use events this is the best way to cleanly prevent the lapsed listener problem
		friend class biggin::Subject;
		std::vector<Subject*> m_pNotifiers{};
	};
}
