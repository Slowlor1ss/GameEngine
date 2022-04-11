#include "BigginPCH.h"
#include "InputManager.h"
#define WIN32_LEAN_AND_MEAN
#include <variant>
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
	void DoHandleInput(int idx);
	bool DoIsPressed(ControllerButton button, int idx) const;
	bool DoIsReleased(ControllerButton button, int idx) const;
	bool DoIsHeld(ControllerButton button, int idx) const;
	bool DoIsPressed(SDL_Keycode key) const;
	bool DoIsReleased(SDL_Keycode key) const;
	bool DoIsHeld(SDL_Keycode key) const;
	glm::vec2 DoGetLThumb(int idx) const;
	glm::vec2 DoGetRThumb(int idx) const;
	float DoGetLTrigger(int idx) const;
	float DoGetRTrigger(int idx) const;
	InputContext DoGetInputContext() const { return m_Context; }

private:
	glm::vec2 ShortToVec2RangeSigned1(short s1, short s2) const;
	float ByteToUnsignedFloat(short s) const;

	XINPUT_STATE m_CurrentState[XUSER_MAX_COUNT]{};
	XINPUT_STATE m_PreviousState[XUSER_MAX_COUNT]{};
	WORD m_ButtonsPressedThisFrame[XUSER_MAX_COUNT]{};
	WORD m_ButtonsReleasedThisFrame[XUSER_MAX_COUNT]{};

	bool m_ConnectedControllers[XUSER_MAX_COUNT];

	const Uint8* m_CurrentkeyState{ SDL_GetKeyboardState(nullptr) };
	//const Uint8* m_PreviouskeyState{ };
	//https://stackoverflow.com/questions/3741055/inputs-in-sdl-on-key-pressed
	//I can't seem to find a better way of doing this
	bool m_PreviousKeyState[322]{}; //TODO: put this on the heap and try uint8* again

	InputContext m_Context{};
};

InputManager::InputManager()
	: m_pImpl(new InputManagerImpl) //TODO: check again for the unordered map
	, m_ConsoleCommands(15, ControllerButtonHash) //bucketSize -> 14 controller keys (+1 for none), as our hash function works with the controller keys we only need 14 
{}

size_t InputManager::ControllerButtonHash(const ActionKey& button)
{
	constexpr std::hash<WORD> buttonHash;
	return buttonHash(static_cast<WORD>(button.button));
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

void InputManager::HandleInput(int i) const
{
	m_pImpl->DoHandleInput(i);
}

bool InputManager::IsHeld(ControllerButton button, int idx) const
{
	return m_pImpl->DoIsHeld(button, idx);
}

bool InputManager::IsPressed(ControllerButton button, int idx) const
{
	return m_pImpl->DoIsPressed(button, idx);
}

bool InputManager::IsReleased(ControllerButton button, int idx) const
{
	return m_pImpl->DoIsReleased(button, idx);
}

glm::vec2 InputManager::GetLThumb(int idx) const
{
	return m_pImpl->DoGetLThumb(idx);
}

glm::vec2 InputManager::GetRThumb(int idx) const
{
	return m_pImpl->DoGetRThumb(idx);
}

float InputManager::GetLTrigger(int idx) const
{
	return m_pImpl->DoGetLTrigger(idx);
}

float InputManager::GetRTrigger(int idx) const
{
	return m_pImpl->DoGetRTrigger(idx);
}

InputManager::InputContext InputManager::GetInputContext() const
{
	return m_pImpl->DoGetInputContext();
}

bool InputManager::InputManagerImpl::DoProcessInput()
{
	CopyMemory(m_PreviousKeyState, m_CurrentkeyState, sizeof(Uint8)*322);
	SDL_Event e;
	while (SDL_PollEvent(&e)) {
		switch (e.type) {
		case SDL_QUIT:
			return false;
			break;
		//case SDL_KEYDOWN:
		//	m_PreviouskeyState[e.key.keysym.sym] = true;
		//	break;
		//case SDL_KEYUP:
		//	m_PreviouskeyState[e.key.keysym.sym] = false;
		//	break;
		default:
			break;
		}
	}

	std::swap(m_PreviousState, m_CurrentState);
	ZeroMemory(&m_CurrentState, sizeof(XINPUT_STATE) * XUSER_MAX_COUNT);

	//m_PreviouskeyState = m_CurrentkeyState;
	//m_CurrentkeyState = SDL_GetKeyboardState(nullptr);

	for (int i = 0; i < XUSER_MAX_COUNT; ++i)
	{
		//check if there's a controller for this idx
		if(XInputGetState(i, &m_CurrentState[i]) == NO_ERROR)
		{
			//new controller added
			m_ConnectedControllers[i] = true;
		}
		else
		{
			//controller removed
			if (m_ConnectedControllers[i])
			{
				//TODO: maybe send a message when this happens that a controller got disconnected and do same for new connected
				m_ConnectedControllers[i] = false;
			}
		}

		//Makes it so the input manager actually detects if the button was just pressed or released so it doesn't stay true for multiple frames
		WORD buttonChanges = m_CurrentState[i].Gamepad.wButtons ^ m_PreviousState[i].Gamepad.wButtons;
		m_ButtonsPressedThisFrame[i] = buttonChanges & m_CurrentState[i].Gamepad.wButtons;
		m_ButtonsReleasedThisFrame[i] = buttonChanges & (~m_CurrentState[i].Gamepad.wButtons);

		DoHandleInput(i);
	}

	return true;
}

void InputManager::InputManagerImpl::DoHandleInput(int idx)
{
	for (const auto& [controllerKey, action] : InputManager::GetInstance().GetConsoleMap())
	{
		if (controllerKey.controllerIdx != idx)
			continue;

		const ActionState& State = controllerKey.state;
		const ControllerButton& button = controllerKey.button;
		const SDL_Keycode& key = controllerKey.key;

		switch (State)
		{
		case ActionState::Up:
			if (DoIsReleased(button, idx) || DoIsReleased(key))
				action->execute();
			break;
		case ActionState::Down:
			if (DoIsPressed(button, idx) || DoIsPressed(key))
				action->execute();
			break;
		case ActionState::Hold:
			if (DoIsHeld(button, idx) || DoIsHeld(key))
				action->execute(); //TODO: add context here aswell for sake of completeness
			break;
		case ActionState::ThumbL:
			if (glm::vec2 v{ DoGetLThumb(idx) }; v != glm::vec2{0,0})
			{
				m_Context = {v, ActionState::ThumbL, idx};
				action->execute();
			}
			break;
		case ActionState::ThumbR:
			if (glm::vec2 v{ DoGetRThumb(idx) }; v != glm::vec2{ 0,0 })
			{
				m_Context = { v, ActionState::ThumbL, idx };
				action->execute();
			}
			break;
		case ActionState::TriggerL:
			if (const float v{ DoGetLTrigger(idx) }; v != 0)
			{
				m_Context = { v, ActionState::TriggerL, idx };
				action->execute();
			}
			break;
		case ActionState::TriggerR:
			if (const float v{ DoGetLTrigger(idx) }; v != 0)
			{
				m_Context = { v, ActionState::TriggerR, idx };
				action->execute();
			}
			break;
		}
		m_Context = {};
	}
}

bool InputManager::InputManagerImpl::DoIsPressed(ControllerButton button, int idx) const
{
	return static_cast<unsigned>(button) & m_ButtonsPressedThisFrame[idx];
}

bool InputManager::InputManagerImpl::DoIsReleased(ControllerButton button, int idx) const
{
	return static_cast<unsigned>(button) & m_ButtonsReleasedThisFrame[idx];
}

bool InputManager::InputManagerImpl::DoIsHeld(ControllerButton button, int idx) const
{
	return m_CurrentState[idx].Gamepad.wButtons & static_cast<unsigned>(button);
}

bool InputManager::InputManagerImpl::DoIsPressed(SDL_Keycode key) const
{
	return m_CurrentkeyState[SDL_GetScancodeFromKey(key)] && !m_PreviousKeyState[SDL_GetScancodeFromKey(key)];
}

bool InputManager::InputManagerImpl::DoIsReleased(SDL_Keycode key) const
{
	return m_PreviousKeyState[SDL_GetScancodeFromKey(key)] && !m_CurrentkeyState[SDL_GetScancodeFromKey(key)];
}

bool InputManager::InputManagerImpl::DoIsHeld(SDL_Keycode key) const
{
	return m_CurrentkeyState[SDL_GetScancodeFromKey(key)];
}

void InputManager::MapActionKey(const ActionKey key, std::unique_ptr<Command> action)
{
	m_ConsoleCommands[key] = std::move(action);
}

glm::vec2 InputManager::InputManagerImpl::ShortToVec2RangeSigned1(short s1, short s2) const
{
	//32767 == max value of a signed short
	constexpr float toFloatRangeSigned1 = 1 / 32767.f;
	//doing a multiplication instead of dividing everytime
	return glm::vec2{ s1 * toFloatRangeSigned1, s2 * toFloatRangeSigned1 };
}

float InputManager::InputManagerImpl::ByteToUnsignedFloat(short s) const
{
	//255 == max value of a byte
	constexpr float toFloatRange1 = 1 / 255.f;
	return s * toFloatRange1;
}

glm::vec2 InputManager::InputManagerImpl::DoGetLThumb(int idx) const
{
	return ShortToVec2RangeSigned1(m_CurrentState[idx].Gamepad.sThumbLX, m_CurrentState[idx].Gamepad.sThumbLY);
}

glm::vec2 InputManager::InputManagerImpl::DoGetRThumb(int idx) const
{
	return ShortToVec2RangeSigned1(m_CurrentState[idx].Gamepad.sThumbRX, m_CurrentState[idx].Gamepad.sThumbRY);
}

float InputManager::InputManagerImpl::DoGetLTrigger(int idx) const
{
	return ByteToUnsignedFloat(m_CurrentState[idx].Gamepad.bLeftTrigger);
}

float InputManager::InputManagerImpl::DoGetRTrigger(int idx) const
{
	return ByteToUnsignedFloat(m_CurrentState[idx].Gamepad.bRightTrigger);
}
