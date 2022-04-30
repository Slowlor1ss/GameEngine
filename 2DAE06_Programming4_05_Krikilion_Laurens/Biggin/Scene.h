#pragma once
#include "SceneManager.h"

namespace biggin
{
	class Transform;
	class GameObject;
	class Scene //TODO: add activate and deactivate
	{
		friend Scene& SceneManager::CreateScene(const std::string& name);
	public:
		~Scene();
		Scene(const Scene& other) = delete;
		Scene(Scene&& other) = delete;
		Scene& operator=(const Scene& other) = delete;
		Scene& operator=(Scene&& other) = delete;

		void Add(const std::shared_ptr<GameObject>& object);

		void Start();
		void Update();
		void FixedUpdate();
		void Render() const;
		void RenderUi();

		const std::string& GetName() const;

	private: 
		explicit Scene(const std::string& name);

		std::string m_Name;
		std::vector < std::shared_ptr<GameObject>> m_Objects{};

		static unsigned int m_IdCounter;
	};

}
