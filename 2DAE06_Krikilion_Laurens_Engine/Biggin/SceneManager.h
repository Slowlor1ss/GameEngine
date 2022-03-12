#pragma once
#include "Singleton.h"

namespace dae
{
	class Transform;
	class Scene;
	class SceneManager final : public Singleton<SceneManager>
	{
	public:
		Scene& CreateScene(const std::string& name);

		void Update();
		void FixedUpdate();
		void Render() const;
		void RenderUi();

	private:
		friend class Singleton<SceneManager>;
		SceneManager() = default;
		std::vector<std::shared_ptr<Scene>> m_Scenes;
	};
}
