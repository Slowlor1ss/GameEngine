#include "BigginPCH.h"
#include "RemoveOnEvent.h"
#include "GameObject.h"
#include <utility>
#include "GameTime.h"
#include "Scene.h"

using namespace biggin;

//https://stackoverflow.com/questions/43196826/should-i-always-use-stdmove-in-a-constructor
RemoveOnEvent::RemoveOnEvent(biggin::GameObject* go, std::string event, std::string nameEventOwningGameObject, float delay)
	:Component(go)
	, m_Event(std::move(event))
	, m_NameEventOwningGameObject(std::move(nameEventOwningGameObject))
	, m_DelayedRemove(delay, {})
	, m_GameTimeRef{ GameTime::GetInstance() }
{
}

//https://stackoverflow.com/questions/43196826/should-i-always-use-stdmove-in-a-constructor
RemoveOnEvent::RemoveOnEvent(biggin::GameObject* go, std::string event, std::shared_ptr<biggin::GameObject> eventOwningGameObject, float delay)
	:Component(go)
	,m_pEventOwner(eventOwningGameObject)
	,m_Event(std::move(event))
	,m_DelayedRemove(delay, {})
	,m_GameTimeRef{ GameTime::GetInstance() }
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

void RemoveOnEvent::Update()
{
	if (m_Remove)
		m_DelayedRemove.Update(m_GameTimeRef.GetDeltaT());
}

void RemoveOnEvent::OnNotify(Component* /*entity*/, const std::string& event)
{
	if (event == m_Event)
	{
		auto go = GetGameObject();
		if (m_DelayedRemove.interval == 0.f)
		{
			go->GetSceneRef()->Remove(go);
		}
		else
		{
			m_Remove = true;
			m_DelayedRemove.func = [go] {go->GetSceneRef()->Remove(go); };
		}
	}
}

