#include "BigginPCH.h"
#include "Scene.h"
#include "GameObject.h"
#include "GameTime.h"
//#include "Box2dManager.h"

using namespace biggin;

unsigned int Scene::m_IdCounter = 0;

Scene::Scene(const std::string& name) : m_Name(name)
{
	//m_SceneContext.GetGameTime()->Start();
}

void Scene::Add(const std::shared_ptr<GameObject>& object)
{
	m_Objects.emplace_back(object);
	object->Initialize(object.get());
}

void biggin::Scene::Start() const
{
	for (auto& object : m_Objects)
	{
		object->Start();
	}
}

void Scene::Update() const
{
	for(auto& object : m_Objects)
	{
		object->Update();
	}
}

void biggin::Scene::FixedUpdate() const
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

