#pragma once
#include "GameObject.h"

namespace dae
{
	class Transform;
}

class Component
{
public:
    virtual ~Component() { /*SafeDelete(m_pGameObject);*/ };
    Component(dae::GameObject* go) : m_pGameObject(go) {};

    virtual void Update() = 0;
    virtual void FixedUpdate() = 0;
    virtual void Render(dae::Transform sceneTransform) const = 0;

protected:
    dae::GameObject* m_pGameObject;
};

