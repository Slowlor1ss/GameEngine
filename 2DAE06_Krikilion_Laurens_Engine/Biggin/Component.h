#pragma once

namespace dae
{
	class GameObject;
	class Transform;
}

class Component
{
public:
    //TODO: maybe not public
    Component(dae::GameObject* go) : m_pGameObject(go) {};
    virtual ~Component() = default;

    Component(const Component& other) = delete;
    Component(Component&& other) noexcept = delete;
    Component& operator=(const Component& other) = delete;
    Component& operator=(Component&& other) noexcept = delete;

    virtual void Start() = 0;
    virtual void Update() = 0;
    virtual void FixedUpdate() = 0;
    virtual void Render() const = 0;
    virtual void RenderUi() = 0;

protected:
    //TODO: make it private and make a protected getter function!
    dae::GameObject* m_pGameObject;
};

