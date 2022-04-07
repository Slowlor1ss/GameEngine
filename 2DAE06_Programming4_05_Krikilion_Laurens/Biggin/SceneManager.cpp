#include "BigginPCH.h"
#include "SceneManager.h"
#include "Scene.h"

void biggin::SceneManager::Update() const
{
	for(auto& scene : m_Scenes)
	{
		scene->Update();
	}
}

void biggin::SceneManager::FixedUpdate() const
{
	for (auto& scene : m_Scenes)
	{
		scene->FixedUpdate();
	}
}

void biggin::SceneManager::Render() const
{
	for (const auto& scene : m_Scenes)
	{
		scene->Render();
	}
}

void biggin::SceneManager::RenderUi()
{
	for (const auto& scene : m_Scenes)
	{
		scene->RenderUi();
	}
}

biggin::Scene& biggin::SceneManager::CreateScene(const std::string& name)
{
	const auto& scene = std::shared_ptr<Scene>(new Scene(name));
	m_Scenes.push_back(scene);
	return *scene;
}
