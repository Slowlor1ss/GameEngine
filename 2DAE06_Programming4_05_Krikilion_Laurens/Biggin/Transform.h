#pragma once
#include "Component.h"

namespace biggin
{
	//TODO: change in to a transform component, add local and relative position and by default add it to the components of gameobject
	class Transform final : public Component
	{
	public:
		Transform(GameObject* go) : Component(go) {};
		~Transform() override = default;

		Transform(const Transform& other) = delete;
		Transform(Transform&& other) noexcept = delete;
		Transform& operator=(const Transform& other) = delete;
		Transform& operator=(Transform&& other) noexcept = delete;

		const glm::vec3& GetLocalPosition() const { return m_LocalPosition; }
		void SetLocalPosition(float x, float y, float z);
		void SetLocalPosition(glm::vec3 pos);
		void TranslateLocalPosition(glm::vec3 pos);

		const glm::vec3& GetWorldPosition() const { return m_WorldPosition; }
		void SetWorldPosition(float x, float y, float z);
		void SetWorldPosition(glm::vec3 pos);
		void TranslateWorldPosition(glm::vec3 pos);

	private:
		glm::vec3 m_WorldPosition{};
		glm::vec3 m_LocalPosition{};
	};
}
