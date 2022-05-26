#pragma once
#include <memory>
#include <string>
#include "Component.h"
#include "Observer.h"

namespace biggin
{
	
class RemoveOnEvent final : public biggin::Component, public biggin::Observer
{
public:
	RemoveOnEvent(biggin::GameObject* go, std::string event, std::string nameEventOwningGameObject);
	RemoveOnEvent(biggin::GameObject* go, std::string event, std::shared_ptr<biggin::GameObject> eventOwningGameObject);
	~RemoveOnEvent() override;

	void Initialize(GameObject*) override;
	void OnNotify(Component* entity, const std::string& event) override;

private:
	std::weak_ptr<biggin::GameObject> m_pEventOwner;
	std::string m_Event;
	std::string m_NameEventOwningGameObject;
};

}
