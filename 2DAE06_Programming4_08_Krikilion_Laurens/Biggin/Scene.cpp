#include "BigginPCH.h"
#include "Scene.h"
#include "GameObject.h"
#include "GameTime.h"

using namespace biggin;

Scene::Scene(const std::string& name) : m_Name(name)
{
	//m_SceneContext.GetGameTime()->Start();
}

void Scene::Add(const std::shared_ptr<GameObject>& object)
{
	object->SetSceneRef(this);
	m_Objects.emplace_back(object);
	object->Initialize(object.get());
}

void Scene::AddPending(const std::shared_ptr<GameObject>& object)
{
	object->SetSceneRef(this);
	m_pPendingAdd.emplace_back(object);
	object->Initialize(object.get()); //TODO: maybe this should be called when it is actually added
}

//Removed is always delayed till after the update as it would be dangerous to instantly remove an object from the scene
void Scene::Remove(const GameObject* object)
{
	m_pPendingDelete.emplace_back(object);
}

std::shared_ptr<GameObject> Scene::FindGameObjectWithName(const std::string& name) const
{
	for (auto& object : m_Objects)
	{
		if (object->Getname() == name)
			return object;
	}
	return nullptr;
}

std::vector<std::shared_ptr<GameObject>> Scene::FindGameObjectsWithName(const std::string& name) const
{
	std::vector<std::shared_ptr<GameObject>> gameObjects;

	for (auto& object : m_Objects)
	{
		if (object->Getname() == name)
			gameObjects.emplace_back(object);
	}

	return gameObjects;
}


void Scene::Start() const
{
	for (auto& object : m_Objects)
	{
		object->Start();
	}
}

void Scene::Update()
{
	for(auto& object : m_Objects)
	{
		object->Update();
	}

	if (!m_pPendingAdd.empty())
	{
		//https://www.geeksforgeeks.org/stdback_inserter-in-cpp/
		std::ranges::move(m_pPendingAdd, std::back_inserter(m_Objects));
		m_pPendingAdd.clear();
	}

	//https://en.cppreference.com/w/cpp/algorithm/remove
	if (!m_pPendingDelete.empty())
	{
		auto pendingDeleteEnd = m_Objects.end();

		for (auto gameObject : m_pPendingDelete)
			pendingDeleteEnd = std::remove_if(m_Objects.begin(), pendingDeleteEnd, [gameObject](const std::shared_ptr<GameObject>& object)
				{
					return object.get() == gameObject;
				});

		m_Objects.erase(pendingDeleteEnd, m_Objects.end());
		m_pPendingDelete.clear();
	}

}

void Scene::FixedUpdate()
{
	if (m_B2Manager != nullptr)
		m_B2Manager->Simulate();

	for (const auto& object : m_Objects)
	{
		object->FixedUpdate();
	}
}

void Scene::Render() const
{
	for (const auto& object : m_Objects)
	{
		object->Render();
	}
}

//void Scene::RenderUi() const
//{
//	for (const auto& object : m_Objects)
//	{
//		object->RenderUi();
//	}
//}

void Scene::RenderDebug() const
{
#ifdef _DEBUG
	for (const auto& object : m_Objects)
	{
 		object->RenderDebug();
	}

	if (m_B2Manager != nullptr)
		m_B2Manager->RenderDebug();
#endif
}

const std::string& biggin::Scene::GetName() const
{
	return m_Name;
}

std::shared_ptr<Box2dManager> biggin::Scene::Getb2Manager()
{
	if (m_B2Manager == nullptr)
		m_B2Manager = std::make_shared<Box2dManager>();

	return m_B2Manager;
}

