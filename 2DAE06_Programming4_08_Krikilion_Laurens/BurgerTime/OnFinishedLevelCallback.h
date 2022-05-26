#pragma once
#include <memory>
#include <string>
#include "Component.h"
#include "Observer.h"

class OnFinishedLevelCallback final : public biggin::Component, public biggin::Observer
{
public:
	OnFinishedLevelCallback(biggin::GameObject* go);
	~OnFinishedLevelCallback() override;

	void OnNotify(Component* entity, const std::string& event) override;

private:
	biggin::GameObject* m_pGameObjectRef;
	std::shared_ptr<biggin::GameObject> m_pLevelRef;
};

