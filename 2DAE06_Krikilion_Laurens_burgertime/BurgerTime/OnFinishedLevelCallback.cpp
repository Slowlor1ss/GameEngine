#include "OnFinishedLevelCallback.h"
#include "GameObject.h"
#include "Scene.h"

OnFinishedLevelCallback::OnFinishedLevelCallback(biggin::GameObject* go)
	: Component(go)
	, m_pGameObjectRef(go)
{
	m_pLevelRef = m_pGameObjectRef->GetSceneRef()->FindGameObjectsWithTag("Map")[0];
	m_pLevelRef->AddObserver(this);
}

OnFinishedLevelCallback::~OnFinishedLevelCallback()
{
	m_pLevelRef->RemoveObserver(this);
}

void OnFinishedLevelCallback::OnNotify(Component* entity, const std::string& event)
{
	if (event == "FinishedLevel")
	{
		
	}
}
