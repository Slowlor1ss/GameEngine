#pragma once
#include "Component.h"

namespace biggin
{
	class Observer;

	class Subject final : public Component
	{
	public:
		Subject(GameObject* go);

		void AddObserver(Observer* observer);
		void RemoveObserver(Observer* observer);

		void notify(const Component* entity, const std::string& event);

	private:
		std::vector<Observer*> m_pObservers{};
		std::vector<Observer*> m_pPendingDelete{};
		bool m_BusyNotifying;
	};
}

