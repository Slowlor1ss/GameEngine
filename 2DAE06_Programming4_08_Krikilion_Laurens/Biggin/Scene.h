#pragma once
#include "SceneManager.h"

class b2World;
#include "Box2dManager.h"

namespace biggin
{
	class Transform;
	class GameObject;
	class Scene //TODO: add activate and deactivate
	{
		friend Scene& SceneManager::CreateScene(const std::string& name);
	public:
		~Scene() = default;
		Scene(const Scene& other) = delete;
		Scene(Scene&& other) = delete;
		Scene& operator=(const Scene& other) = delete;
		Scene& operator=(Scene&& other) = delete;

		void Add(const std::shared_ptr<GameObject>& object);

		void Start() const;
		void Update() const;
		void FixedUpdate() const;
		void Render() const;
		//void RenderUi() const;
		void RenderDebug() const;

		const std::string& GetName() const;

		std::shared_ptr<Box2dManager> Getb2Manager();

	private: 
		explicit Scene(const std::string& name);

		std::string m_Name;
		std::vector <std::shared_ptr<GameObject>> m_Objects{};

		std::shared_ptr<Box2dManager> m_B2Manager{nullptr};

		static unsigned int m_IdCounter;
	};

}
