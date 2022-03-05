#pragma once
#include "GameObject.h"

namespace dae
{
	class Transform;
}

class Component
{
public:
    Component(dae::GameObject* go) : m_pGameObject(go) {};
    virtual ~Component() = default;

    Component(const Component& other) = delete;
    Component(Component&& other) noexcept = delete;
    Component& operator=(const Component& other) = delete;
    Component& operator=(Component&& other) noexcept = delete;

    virtual void Update() = 0;
    virtual void FixedUpdate() = 0;
    virtual void Render(dae::Transform sceneTransform) const = 0;

protected:
    dae::GameObject* m_pGameObject;
};

