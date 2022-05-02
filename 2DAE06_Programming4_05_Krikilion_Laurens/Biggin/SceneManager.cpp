#include "BigginPCH.h"
#include "SceneManager.h"
#include "Scene.h"

void biggin::SceneManager::Update() const
{
	//for(auto& scene : m_Scenes)
	//{
	//	scene->Update();
	//}

	if (m_ActiveScene != nullptr)
	{
		m_ActiveScene->Update();
	}
}

void biggin::SceneManager::FixedUpdate() const
{
	//for (auto& scene : m_Scenes)
	//{
	//	scene->FixedUpdate();
	//}
	m_ActiveScene->FixedUpdate();
}

void biggin::SceneManager::Render() const
{
	//for (const auto& scene : m_Scenes)
	//{
	//	scene->Render();
	//}
	m_ActiveScene->Render();
}

void biggin::SceneManager::RenderUi() const
{
	//for (const auto& scene : m_Scenes)
	//{
	//	scene->RenderUi();
	//}
	m_ActiveScene->RenderUi();
}

bool biggin::SceneManager::ChangeActiveScene(const std::string& name)
{
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
	const auto& scene = std::shared_ptr<Scene>(new Scene(name));
	m_Scenes.push_back(scene);

	if (!m_HasActiveScene)
		m_ActiveScene = scene;

	return *scene;
}
