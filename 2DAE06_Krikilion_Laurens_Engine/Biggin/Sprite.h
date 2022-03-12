#pragma once
#include "Component.h"

namespace dae
{
	class Texture2D;
	class Transform;

	class RenderComponent : public Component
	{
	public:
		RenderComponent(dae::GameObject* go, std::string path);
		RenderComponent(dae::GameObject* go);

		void Start() override {};
		void Update() override {};
		void FixedUpdate() override {};
		void Render() const override;
		void RenderUi() override {};

		void SetTexture(std::shared_ptr<dae::Texture2D> texture) { m_Texture = texture; }

	private:
		const Transform* m_Transform;
		std::shared_ptr<dae::Texture2D> m_Texture{nullptr};
	};

}
