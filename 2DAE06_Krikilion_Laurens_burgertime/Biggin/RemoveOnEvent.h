#pragma once
#include <memory>
#include <string>
#include "Component.h"
#include "Observer.h"
#include "Utils.hpp"


namespace biggin
{
	class GameTime;
	
class RemoveOnEvent final : public biggin::Component, public biggin::Observer
{
public:
	RemoveOnEvent(biggin::GameObject* go, std::string event, std::string nameEventOwningGameObject, float delay=0);
	RemoveOnEvent(biggin::GameObject* go, std::string event, std::shared_ptr<biggin::GameObject> eventOwningGameObject, float delay=0);
	~RemoveOnEvent() override;

	RemoveOnEvent(const RemoveOnEvent& other) = delete;
	RemoveOnEvent(RemoveOnEvent&& other) noexcept = delete;
	RemoveOnEvent& operator=(const RemoveOnEvent& other) = delete;
	RemoveOnEvent& operator=(RemoveOnEvent&& other) noexcept = delete;

	void Initialize(GameObject*) override;
	void OnNotify(Component* entity, const std::string& event) override;
	void Update() override;

private:
	std::weak_ptr<biggin::GameObject> m_pEventOwner;
	std::string m_Event;
	std::string m_NameEventOwningGameObject;
	utils::DelayedCallback m_DelayedRemove;
	bool m_Remove{ false };
	GameTime& m_GameTimeRef;
};

}
