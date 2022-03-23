#include "BigginPCH.h"
#include "InputManager.h"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Xinput.h>
#include "Command.h"

#pragma comment(lib, "xinput.lib")

using namespace biggin;

class InputManager::InputManagerImpl
{
public:
	InputManagerImpl() = default;

	bool DoProcessInput();
	void DoHandleInput() const;
	bool DoIsPressed(ControllerButton button) const;
	bool DoIsReleased(ControllerButton button) const;
	bool DoIsHeld(ControllerButton button) const;

private:
	XINPUT_STATE m_CurrentState{};
	XINPUT_STATE m_PreviousState{};
	unsigned int m_ButtonsPressedThisFrame{};
	unsigned int m_ButtonsReleasedThisFrame{};
};

InputManager::InputManager()
	: m_pImpl(new InputManagerImpl)
	, m_ConsoleCommands(14, ControllerButtonHash) //bucketSize -> 14 controller keys, as our hash function works with the controller keys we only need 14 
{}

size_t InputManager::ControllerButtonHash(const ControllerKey& button)
{
	constexpr std::hash<WORD> buttonHash;
	return buttonHash(static_cast<WORD>(button.second));
}

InputManager::~InputManager()
{
	delete m_pImpl;
	m_pImpl = nullptr;
}

bool InputManager::ProcessInput() const
{
	return m_pImpl->DoProcessInput();
}

void InputManager::HandleInput() const
{
	m_pImpl->DoHandleInput();
}

bool InputManager::IsHeld(ControllerButton button) const
{
	return m_pImpl->DoIsHeld(button);
}

bool InputManager::IsPressed(ControllerButton button) const
{
	return m_pImpl->DoIsPressed(button);
}

bool InputManager::IsReleased(ControllerButton button) const
{
	return m_pImpl->DoIsReleased(button);
}

bool InputManager::InputManagerImpl::DoProcessInput()
{
	CopyMemory(&m_PreviousState, &m_CurrentState, sizeof(XINPUT_STATE));
	ZeroMemory(&m_CurrentState, sizeof(XINPUT_STATE));
	XInputGetState(0, &m_CurrentState);

	//Makes it so the input manager actually detects if the button was just pressed or released so it doesn't stay true for multiple frames
	auto buttonChanges = m_CurrentState.Gamepad.wButtons ^ m_PreviousState.Gamepad.wButtons;
	m_ButtonsPressedThisFrame = buttonChanges & m_CurrentState.Gamepad.wButtons;
	m_ButtonsReleasedThisFrame = buttonChanges & (~m_CurrentState.Gamepad.wButtons);

	SDL_Event e;
	while (SDL_PollEvent(&e)) {
		if (e.type == SDL_QUIT) {
			return false;
		}
	}

	return true;
}

void InputManager::InputManagerImpl::DoHandleInput() const
{
	for (const auto& [controllerKey, action] : InputManager::GetInstance().GetConsoleMap())
	{
		const ButtonState& keyState = controllerKey.first;
		const ControllerButton& button = controllerKey.second;

		switch (keyState)
		{
		case ButtonState::Up:
			if (DoIsReleased(button))
				action->execute();
			break;
		case ButtonState::Down:
			if (DoIsPressed(button))
				action->execute();
			break;
		case ButtonState::Hold:
			if (DoIsHeld(button))
				action->execute();
			break;
		}

	}
}

bool InputManager::InputManagerImpl::DoIsPressed(ControllerButton button) const
{
	return static_cast<unsigned>(button) & m_ButtonsPressedThisFrame;
}

bool InputManager::InputManagerImpl::DoIsReleased(ControllerButton button) const
{
	return static_cast<unsigned>(button) & m_ButtonsReleasedThisFrame;
}

bool InputManager::InputManagerImpl::DoIsHeld(ControllerButton button) const
{
	return m_CurrentState.Gamepad.wButtons & static_cast<unsigned>(button);
}

void InputManager::MapActionKey(ControllerKey key, std::unique_ptr<Command> action)
{
	m_ConsoleCommands[key] = std::move(action);
}
