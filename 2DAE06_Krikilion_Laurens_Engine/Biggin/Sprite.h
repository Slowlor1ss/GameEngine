#pragma once
#include "Component.h"
#include "Transform.h"

namespace dae
{
	class Texture2D;
}

class Sprite : public Component
{
public:
	Sprite(std::string path, dae::Transform transform = dae::Transform{});

	void Update() override {};//TODO::Implement
	void FixedUpdate(float) override {};
	void Render(dae::Transform sceneTransform) const override;

private:
	dae::Transform m_Transform;
	std::shared_ptr<dae::Texture2D> m_Texture{nullptr};
};

