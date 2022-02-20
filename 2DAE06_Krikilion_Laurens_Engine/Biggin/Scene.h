#pragma once
#include "SceneContext.h"
#include "SceneManager.h"

namespace dae
{
	class Transform;
	class GameObject;
	class Scene //TODO: add activate and deactivate
	{
		friend Scene& SceneManager::CreateScene(const std::string& name);
	public:
		void Add(const std::shared_ptr<GameObject>& object);

		void Update();
		void FixedUpdate(float fixedTimeStep);
		void Render() const;

		const SceneContext& GetSceneContext() const { return m_SceneContext; }

		~Scene();
		Scene(const Scene& other) = delete;
		Scene(Scene&& other) = delete;
		Scene& operator=(const Scene& other) = delete;
		Scene& operator=(Scene&& other) = delete;

	private: 
		explicit Scene(const std::string& name);

		std::string m_Name;
		std::vector < std::shared_ptr<GameObject>> m_Objects{};

		static unsigned int m_IdCounter;

		SceneContext m_SceneContext{};
	};

}
