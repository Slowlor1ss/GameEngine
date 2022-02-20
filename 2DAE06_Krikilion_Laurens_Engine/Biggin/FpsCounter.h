#pragma once
#include "Component.h"

namespace dae
{
	class Scene;
	class TextObject;
}

class FpsCounter final : public Component
{
public:
	FpsCounter(dae::Scene& scene);
	~FpsCounter() override;

	FpsCounter(const FpsCounter& other) = delete;
	FpsCounter(FpsCounter&& other) = delete;
	FpsCounter& operator=(const FpsCounter& other) = delete;
	FpsCounter& operator=(FpsCounter&& other) = delete;

	void Update() override;
	void FixedUpdate(float) override {};
	void Render(dae::Transform sceneTransform) const override;

private:
	dae::TextObject* m_pFpsText;
	dae::Scene& m_Scene;
};

