#pragma once

namespace dae
{
	class GameObject;
	class Transform;
}

//TODO:: rewrite this
class Component
{

public:
    virtual ~Component() = default;
    Component() = default;

    virtual void Update() = 0;
    virtual void FixedUpdate(float fixedTimeStep) = 0;
    virtual void Render(dae::Transform sceneTransform) const = 0;

};

