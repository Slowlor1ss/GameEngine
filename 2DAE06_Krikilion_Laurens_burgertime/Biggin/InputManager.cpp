#include "BigginPCH.h"
#include <vector>
#include "InputManager.h"
#define WIN32_LEAN_AND_MEAN
#include <variant>
#include <Windows.h>
#include <Xinput.h>
#include "Command.h"
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include <SDL_keycode.h>

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
	void BindImguiButton(ImGuiIO& io, ControllerButton button, SDL_Keycode key, int idx) const;

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
	static constexpr int m_SizeOfKeyStates{ 322 };
	bool m_PreviousKeyState[m_SizeOfKeyStates]{};

	InputContext m_Context{};
};

InputManager::InputManager()
	: m_pImpl(new InputManagerImpl) //should check again for the unordered map since we added a keyboard
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

bool InputManager::IsHeld(SDL_Keycode key, ControllerButton button, int idx) const
{
	return m_pImpl->DoIsHeld(button, idx) || m_pImpl->DoIsHeld(key);
}

bool InputManager::IsPressed(SDL_Keycode key, ControllerButton button, int idx) const
{
	return m_pImpl->DoIsPressed(button, idx) || m_pImpl->DoIsPressed(key);
}

bool InputManager::IsReleased(SDL_Keycode key, ControllerButton button, int idx) const
{
	return m_pImpl->DoIsReleased(button, idx) || m_pImpl->DoIsReleased(key);
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

//this is the only way i managed to come up with to make navigation work with controller,
//I tried for hours to make it work with the official implementation but it didn't work for me
void InputManager::InputManagerImpl::BindImguiButton(ImGuiIO& io, ControllerButton button, SDL_Keycode key, int idx) const
{
	if (!io.KeysDown[SDL_GetScancodeFromKey(key)] && DoIsPressed(button, idx))
	{
		io.KeysDown[SDL_GetScancodeFromKey(key)] = true;
	}
	else if (DoIsReleased(button, idx))
	{
		io.KeysDown[SDL_GetScancodeFromKey(key)] = false;
	}
}

bool InputManager::InputManagerImpl::DoProcessInput()
{
	CopyMemory(m_PreviousKeyState, m_CurrentkeyState, sizeof(Uint8) * m_SizeOfKeyStates);
	SDL_Event e;
	while (SDL_PollEvent(&e)) {
		ImGui_ImplSDL2_ProcessEvent(&e); //enables keyboard for imgui
		switch (e.type) {
		case SDL_QUIT:
			return false;
			break;
		case SDL_KEYDOWN:
		{
			//int key = e.key.keysym.scancode;
			//m_CurrentkeyState[key] = (e.type == SDL_KEYDOWN);
			//			KeyShift = ((SDL_GetModState() & KMOD_SHIFT) != 0);
			//			KeyCtrl = ((SDL_GetModState() & KMOD_CTRL) != 0);
			//			KeyAlt = ((SDL_GetModState() & KMOD_ALT) != 0);
			//#ifdef _WIN32
			//			KeySuper = false;
			//#else
			//			io.KeySuper = ((SDL_GetModState() & KMOD_GUI) != 0);
			//#endif
		}
		default:
			break;
		}
	}

	std::swap(m_PreviousState, m_CurrentState);
	ZeroMemory(&m_CurrentState, sizeof(XINPUT_STATE) * XUSER_MAX_COUNT);

	ImGuiIO& io = ImGui::GetIO();
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
		const WORD buttonChanges = m_CurrentState[i].Gamepad.wButtons ^ m_PreviousState[i].Gamepad.wButtons;
		m_ButtonsPressedThisFrame[i] = buttonChanges & m_CurrentState[i].Gamepad.wButtons;
		m_ButtonsReleasedThisFrame[i] = buttonChanges & (~m_CurrentState[i].Gamepad.wButtons);

		BindImguiButton(io, ControllerButton::DpadDown, SDLK_DOWN, i);
		BindImguiButton(io, ControllerButton::DpadUp, SDLK_UP, i);
		BindImguiButton(io, ControllerButton::DpadLeft, SDLK_LEFT, i);
		BindImguiButton(io, ControllerButton::DpadRight, SDLK_RIGHT, i);
		BindImguiButton(io, ControllerButton::ButtonA, SDLK_SPACE, i);

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
			{
				m_Context = { 1.f, ActionState::Up, idx }; //mainly for sake of completeness
				action->execute();
			}
			break;
		case ActionState::Down:
			if (DoIsPressed(button, idx) || DoIsPressed(key))
			{
				m_Context = { 1.f, ActionState::Down, idx }; //mainly for sake of completeness
				action->execute();
			}
			break;
		case ActionState::Hold:
			if (DoIsHeld(button, idx) || DoIsHeld(key))
			{
				m_Context = { 1.f, ActionState::Hold, idx }; //mainly for sake of completeness
				action->execute();
			}
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
	const SDL_Scancode scancodeKey = SDL_GetScancodeFromKey(key);
#ifdef _DEBUG
	if (scancodeKey > m_SizeOfKeyStates || scancodeKey < 0)
		throw std::runtime_error("KeyState accessing array out of bounds");
#endif

	return m_CurrentkeyState[static_cast<UINT8>(scancodeKey)] && !m_PreviousKeyState[scancodeKey];
}

bool InputManager::InputManagerImpl::DoIsReleased(SDL_Keycode key) const
{
	const SDL_Scancode scancodeKey = SDL_GetScancodeFromKey(key);
#ifdef _DEBUG
	if (scancodeKey > m_SizeOfKeyStates || scancodeKey < 0)
		throw std::runtime_error("KeyState accessing array out of bounds");
#endif

	return m_PreviousKeyState[static_cast<UINT8>(scancodeKey)] && !m_CurrentkeyState[scancodeKey];
}

bool InputManager::InputManagerImpl::DoIsHeld(SDL_Keycode key) const
{
	const SDL_Scancode scancodeKey = SDL_GetScancodeFromKey(key);
#ifdef _DEBUG
	if (scancodeKey > m_SizeOfKeyStates || scancodeKey < 0)
		throw std::runtime_error("KeyState accessing array out of bounds");
#endif

	return m_CurrentkeyState[scancodeKey];
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
