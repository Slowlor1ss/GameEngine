#pragma once
#include "Component.h"

namespace dae
{
	class Scene;
	class TextComponent;
}

class FpsCounter final : public Component
{
public:
	FpsCounter(dae::GameObject* go);
	~FpsCounter() override;

	FpsCounter(const FpsCounter& other) = delete;
	FpsCounter(FpsCounter&& other) = delete;
	FpsCounter& operator=(const FpsCounter& other) = delete;
	FpsCounter& operator=(FpsCounter&& other) = delete;

	void Update() override;
	void FixedUpdate() override {};
	void Render(dae::Transform sceneTransform) const override;

private:
	std::shared_ptr<dae::TextComponent> m_pFpsText;
};

