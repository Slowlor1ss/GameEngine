#pragma once
#include "Component.h"
#pragma warning(push)
#pragma warning (disable:4201)
#include <glm/glm.hpp>
#pragma warning(pop)

namespace biggin
{
	class Transform final : public Component
	{
	public:
		Transform(GameObject* go) : Component(go) {};
		~Transform() override = default;

		Transform(const Transform& other) = delete;
		Transform(Transform&& other) noexcept = delete;
		Transform& operator=(const Transform& other) = delete;
		Transform& operator=(Transform&& other) noexcept = delete;

		const glm::vec2 GetLocalPosition() const { return {m_LocalPosition.x, m_LocalPosition.y}; }
		void SetLocalPosition(float x, float y, float z = 0);
		void SetLocalPosition(glm::vec3 pos);
		void SetLocalPosition(glm::vec2 pos);
		void TranslateLocalPosition(glm::vec3 pos);
		void TranslateLocalPosition(glm::vec2 pos);

		const glm::vec2 GetWorldPosition() const { return {m_WorldPosition.x, m_WorldPosition.y}; }
		void SetWorldPosition(float x, float y, float z = 0);
		void SetWorldPosition(glm::vec3 pos);
		void SetWorldPosition(glm::vec2 pos);
		void TranslateWorldPosition(glm::vec3 pos);
		void TranslateWorldPosition(glm::vec2 pos);

	private:
		glm::vec3 m_WorldPosition{};
		glm::vec3 m_LocalPosition{};
	};
}
