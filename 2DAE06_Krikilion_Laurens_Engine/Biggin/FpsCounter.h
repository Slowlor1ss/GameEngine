#pragma once
#include "Component.h"

namespace biggin
{
	class Scene;
	class TextComponent;

class FpsCounter final : public Component
{
public:
	FpsCounter(biggin::GameObject* go);
	~FpsCounter() override = default;

	FpsCounter(const FpsCounter& other) = delete;
	FpsCounter(FpsCounter&& other) = delete;
	FpsCounter& operator=(const FpsCounter& other) = delete;
	FpsCounter& operator=(FpsCounter&& other) = delete;

	void Update() override;

private:
	biggin::TextComponent* m_pFpsText;
};

}
