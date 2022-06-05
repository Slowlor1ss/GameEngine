#include "BigginPCH.h"
#include "SceneManager.h"
#include <algorithm>
#include "Scene.h"

void biggin::SceneManager::Update() const
{
	if (m_ActiveScene != nullptr && !m_Paused)
	{
		m_ActiveScene->Update();
	}
}

void biggin::SceneManager::FixedUpdate() const
{
	if (m_ActiveScene != nullptr && !m_Paused)
		m_ActiveScene->FixedUpdate();
}

void biggin::SceneManager::Render() const
{
	if (m_ActiveScene != nullptr)
		m_ActiveScene->Render();
}

void biggin::SceneManager::RenderDebug() const
{
	if (m_ActiveScene != nullptr)
		m_ActiveScene->RenderDebug();
}

bool biggin::SceneManager::ChangeActiveScene(const std::string& name, bool unPause)
{
	m_Paused = !unPause;

	//if scene already active
	if (m_ActiveScene && m_ActiveScene->GetName() == name)
		return true;

	//search for scene
	const auto sceneIt = 
		std::ranges::find_if(std::as_const(m_Scenes), [&](const std::shared_ptr<Scene>& scene)
		{
			return (scene->GetName() == name);
		});

	//if scene is found return true
	if (sceneIt != m_Scenes.end())
	{
		m_ActiveScene = *sceneIt;
		return true;
	}

	return false;
}

biggin::Scene& biggin::SceneManager::GetActiveScene() const
{
	return *m_ActiveScene;
}

biggin::Scene& biggin::SceneManager::CreateScene(const std::string& name)
{
	//check if scene already exists
	const auto sceneIt =
		std::ranges::find_if(std::as_const(m_Scenes), [&](const std::shared_ptr<Scene>& scene)
			{
				return (scene->GetName() == name);
			});

	//if scene is found return that scene
	if (sceneIt != m_Scenes.end())
	{
		m_ActiveScene = *sceneIt;
		return *m_ActiveScene;
	}

	const auto& scene = std::shared_ptr<Scene>(new Scene(name));
	//const auto& scene = std::make_shared<Scene>(name);
	m_Scenes.emplace_back(scene);

	if (!m_HasActiveScene)
		m_ActiveScene = scene;

	return *scene;
}

void biggin::SceneManager::RemoveScene(const std::string& name)
{
	if (name == m_ActiveScene.get()->GetName())
		m_ActiveScene = nullptr;

	std::erase_if(m_Scenes, [&](const std::shared_ptr<Scene>& scene){ return (scene->GetName() == name); });
}
