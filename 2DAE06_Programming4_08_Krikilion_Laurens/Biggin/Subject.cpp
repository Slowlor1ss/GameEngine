#include "BigginPCH.h"
#include "Subject.h"
#include "Observer.h"

biggin::Subject::Subject(GameObject* go) : Component(go)
	, m_BusyNotifying(false)
{
}

void biggin::Subject::AddObserver(Observer* observer)
{
	const auto it = std::ranges::find(m_pObservers, observer);
	if (it == m_pObservers.end())
		m_pObservers.push_back(observer);
}

//https://gameprogrammingpatterns.com/observer.html#the-subject
//As mentioned in the book you have to watch out for deletion while notifying the list of observers
void biggin::Subject::RemoveObserver(Observer* observer)
{
	//Prevent observers from modifying the list(m_pObservers) in their onNotify() methods
	if (m_BusyNotifying)
	{
		m_pPendingDelete.push_back(observer);
	}
	else //not notifying so its safe to delete
	{
		const auto it = std::ranges::find(m_pObservers, observer);
		if (it != m_pObservers.end())
			m_pObservers.erase(it);
	}
}

void biggin::Subject::notify(Component* entity, const std::string& event)
{
	m_BusyNotifying = true;
	for (auto* pObserver : m_pObservers)
	{
		pObserver->OnNotify(entity, event);
	}

	//https://en.cppreference.com/w/cpp/algorithm/remove
	if (!m_pPendingDelete.empty())
	{
		auto pendingDeleteEnd = m_pObservers.end();

		for (auto* pObserver : m_pPendingDelete)
			pendingDeleteEnd = std::remove(m_pObservers.begin(), pendingDeleteEnd, pObserver);

		m_pObservers.erase(pendingDeleteEnd, m_pObservers.end());
		m_pPendingDelete.clear();
	}
	m_BusyNotifying = false;
}
