#include "BigginPCH.h"
#include "MenuRenderer.h"
#include "MenuState.h"

void biggin::MenuRenderer::SetMenu(MenuState* menu)
{
	delete m_pState;
	m_pState = menu;
}

void biggin::MenuRenderer::RenderMenu() const
{
	m_pState->RenderMenu();
}

biggin::MenuRenderer::~MenuRenderer()
{
	delete m_pState;
	m_pState = nullptr;
}
