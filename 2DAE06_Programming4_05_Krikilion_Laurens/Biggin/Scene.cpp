#include "BigginPCH.h"
#include "Scene.h"
#include "GameObject.h"
#include "GameTime.h"
#include "GameObject.h"

using namespace biggin;

unsigned int Scene::m_IdCounter = 0;

Scene::Scene(const std::string& name) : m_Name(name)
{
	//m_SceneContext.GetGameTime()->Start();
}

Scene::~Scene() = default;

void Scene::Add(const std::shared_ptr<GameObject>& object)
{
	m_Objects.push_back(object);
	object->Initialize(object.get());
}

void biggin::Scene::Start()
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
}

void biggin::Scene::FixedUpdate()
{
	for (auto& object : m_Objects)
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

void Scene::RenderUi()
{
	for (const auto& object : m_Objects)
	{
		object->RenderUi();
	}
}

const std::string& biggin::Scene::GetName() const
{
	return m_Name;
}

