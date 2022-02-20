#include "BigginPCH.h"
#include "SceneContext.h"
#include "GameTime.h"
#include "InputManager.h"

SceneContext::SceneContext() :
	m_pGameTime{ new GameTime() },
	m_pInput{ new dae::InputManager() }
{

}

SceneContext::~SceneContext()
{
	SafeDelete(m_pGameTime);
	SafeDelete(m_pInput);
}
