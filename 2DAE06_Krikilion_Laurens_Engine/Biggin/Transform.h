#pragma once
#include "Component.h"

namespace dae
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

		const glm::vec3& GetPosition() const { return m_Position; }
		void SetPosition(float x, float y, float z);
		void Start() override {};
		void Update() override {};
		void FixedUpdate() override {};
		void Render() const override {};
		void RenderUi() override {};

	private:
		glm::vec3 m_Position{};
	};
}
