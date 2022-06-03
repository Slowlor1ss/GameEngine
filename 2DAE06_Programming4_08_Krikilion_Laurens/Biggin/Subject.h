#pragma once
#include "Component.h"
#include <vector>

namespace biggin
{
	class Observer;

	class Subject final : public Component
	{
	public:
		Subject(GameObject* go);
		~Subject() override;

		Subject(const Subject& other) = delete;
		Subject(Subject&& other) noexcept = delete;
		Subject& operator=(const Subject& other) = delete;
		Subject& operator=(Subject&& other) noexcept = delete;

		void AddObserver(Observer* observer);
		void RemoveObserver(Observer* observer);

		void notify(Component* entity, const std::string& event);

	private:
		std::vector<Observer*> m_pObservers{};
		std::vector<Observer*> m_pPendingDelete{};
		bool m_BusyNotifying;
	};
}

