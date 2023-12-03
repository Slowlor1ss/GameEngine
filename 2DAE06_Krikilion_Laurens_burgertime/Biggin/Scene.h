#pragma once
#include "SceneManager.h"

class b2World;
#include "Box2dManager.h"
#include <vector>
#include <string>

namespace biggin
{
	class GameObject;
	class Scene
	{
		friend Scene& SceneManager::CreateScene(const std::string& name);
	public:
		~Scene() = default;
		Scene(const Scene& other) = delete;
		Scene(Scene&& other) = delete;
		Scene& operator=(const Scene& other) = delete;
		Scene& operator=(Scene&& other) = delete;

		void Start() const;
		void Update();
		void FixedUpdate();
		void Render() const;
		//void RenderUi() const;
		void RenderDebug() const;

		void Add(const std::shared_ptr<GameObject>& object);
		void AddPending(const std::shared_ptr<GameObject>& object);
		void Remove(const GameObject* object);

		std::shared_ptr<GameObject> FindGameObjectWithName(const std::string& name) const;
		std::vector<std::shared_ptr<GameObject>> FindGameObjectsWithName(const std::string& name) const;

		const std::string& GetName() const;

		std::shared_ptr<Box2dManager> Getb2Manager();

	private: 
		explicit Scene(const std::string& name);

		std::string m_Name{};
		std::vector <std::shared_ptr<GameObject>> m_Objects{};
		std::vector <const GameObject*> m_pPendingDelete{};
		std::vector <std::shared_ptr<GameObject>> m_pPendingAdd{};

		std::shared_ptr<Box2dManager> m_B2Manager{nullptr};
	};

}
