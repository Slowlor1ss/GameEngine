#pragma once
#include "Component.h"

namespace biggin
{
	class Texture2D;
	class Transform;

	class RenderComponent : public Component
	{
	public:
		RenderComponent(biggin::GameObject* go, std::string path, const glm::vec2& offset = { 0, 0 });
		RenderComponent(biggin::GameObject* go);

		void Start() override {};
		void Update() override {};
		void FixedUpdate() override {};
		void Render() const override;
		void RenderUi() override {};

		void RenderWithOffset(const glm::vec2& Offset) const;
		void SetTexture(std::shared_ptr<biggin::Texture2D> texture) { m_pTexture = texture; }

	private:
		glm::vec2 m_Offset;
		std::shared_ptr<Texture2D> m_pTexture{nullptr};

		const Transform* m_Transform;
	};

}
