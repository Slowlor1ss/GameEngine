#pragma once
#include "Singleton.h"

namespace biggin
{
	class Transform;
	class Scene;
	class SceneManager final : public Singleton<SceneManager>
	{
	public:
		Scene& CreateScene(const std::string& name);

		void Update() const;
		void FixedUpdate() const;
		void Render() const;
		void RenderUi() const;

		bool ChangeActiveScene(const std::string& name);
		biggin::Scene& GetActiveScene() const;

	private:
		friend class Singleton<SceneManager>;
		SceneManager() = default;
		std::shared_ptr<Scene> m_ActiveScene;
		std::vector<std::shared_ptr<Scene>> m_Scenes;
		bool m_HasActiveScene{ false };
	};
}
