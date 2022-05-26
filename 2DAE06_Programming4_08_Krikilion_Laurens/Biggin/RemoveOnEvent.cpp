#include "BigginPCH.h"
#include "RemoveOnEvent.h"
#include "GameObject.h"
#include <utility>
#include "Scene.h"

using namespace biggin;

//https://stackoverflow.com/questions/43196826/should-i-always-use-stdmove-in-a-constructor
RemoveOnEvent::RemoveOnEvent(biggin::GameObject* go, std::string event, std::string nameEventOwningGameObject)
	:Component(go)
	, m_Event(std::move(event))
	, m_NameEventOwningGameObject(std::move(nameEventOwningGameObject))
{
}

//https://stackoverflow.com/questions/43196826/should-i-always-use-stdmove-in-a-constructor
RemoveOnEvent::RemoveOnEvent(biggin::GameObject* go, std::string event, std::shared_ptr<biggin::GameObject> eventOwningGameObject)
	:Component(go)
	,m_pEventOwner(eventOwningGameObject)
	,m_Event(std::move(event))
{
	m_pEventOwner.lock()->AddObserver(this);
}

void RemoveOnEvent::Initialize(GameObject* go)
{
	if (m_pEventOwner.lock() == nullptr)
	{
		m_pEventOwner = go->GetSceneRef()->FindGameObjectWithName(m_NameEventOwningGameObject);
		m_pEventOwner.lock()->AddObserver(this);
	}
}

RemoveOnEvent::~RemoveOnEvent()
{
	if (!m_pEventOwner.expired())
		m_pEventOwner.lock()->RemoveObserver(this);
}

void RemoveOnEvent::OnNotify(Component* /*entity*/, const std::string& event)
{
	if (event == m_Event)
	{
		auto go = GetGameObject();
		go->GetSceneRef()->Remove(go);
	}
}