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
	~FpsCounter() override = default;

	FpsCounter(const FpsCounter& other) = delete;
	FpsCounter(FpsCounter&& other) = delete;
	FpsCounter& operator=(const FpsCounter& other) = delete;
	FpsCounter& operator=(FpsCounter&& other) = delete;

	void Start() override {};
	void Update() override;
	void FixedUpdate() override {};
	void Render() const override;
	void RenderUi() override {};

private:
	std::shared_ptr<dae::TextComponent> m_pFpsText;
};

