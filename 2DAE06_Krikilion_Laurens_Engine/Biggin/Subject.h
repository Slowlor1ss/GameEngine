#pragma once

namespace biggin
{
	class Component;
	class Observer;

	class Subject
	{
	public:
		Subject() = default;

		void AddObserver(Observer* observer);
		void RemoveObserver(Observer* observer);

	protected:
		void notify(const Component* entity, const std::string& event) const;

	private:
		std::vector<Observer*> m_pObservers;
	};
}

