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
	Sprite(dae::GameObject* go, std::string path, dae::Transform transform = dae::Transform{});
	Sprite(dae::GameObject* go);

	void Update() override {};//TODO::Implement
	void FixedUpdate() override {};
	void Render(dae::Transform sceneTransform) const override;

	void SetTexture(std::shared_ptr<dae::Texture2D> texture) { m_Texture = texture; }
	void SetTransform(dae::Transform transform) { m_Transform = transform; };
	void SetPos(float x, float y, float z) { m_Transform.SetPosition(x, y, z); };

private:
	dae::Transform m_Transform;
	std::shared_ptr<dae::Texture2D> m_Texture{nullptr};
};

