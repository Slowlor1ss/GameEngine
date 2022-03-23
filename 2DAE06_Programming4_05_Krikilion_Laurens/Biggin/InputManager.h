#pragma once
#include <functional>
#include <memory>
#include <unordered_map>
#include <utility>
#include "Singleton.h"

class Command;

namespace biggin
{
	enum class ControllerButton : WORD
	{
		DpadUp = 0x0001,
		DpadDown = 0x0002,
		DpadLeft = 0x0004,
		DpadRight = 0x0008,
		Start = 0x0010,
		Back = 0x0020,
		LeftThumb = 0x0040,
		RightThumb = 0x0080,
		LeftShoulder = 0x0100,
		RightShoulder = 0x0200,
		ButtonA = 0x1000,
		ButtonB = 0x2000,
		ButtonX = 0x4000,
		ButtonY = 0x8000,
	};

	enum class ButtonState
	{
		Up,
		Down,
		Hold
	};

	//TODO: add keyboard
	class InputManager final : public Singleton<InputManager>
	{
		class InputManagerImpl;
		InputManagerImpl* m_pImpl;

		using ControllerKey = std::pair<ButtonState, ControllerButton>;
		using ControllerCommandMap = std::unordered_map<const ControllerKey, std::unique_ptr<Command>, std::function<size_t(const ControllerKey&)>>;

	public:
		InputManager();
		~InputManager() override;

		InputManager(const InputManager& other) = delete;
		InputManager(InputManager&& other) noexcept = delete;
		InputManager& operator=(const InputManager& other) = delete;
		InputManager& operator=(InputManager&& other) noexcept = delete;

		bool ProcessInput() const;
		void HandleInput() const;
		bool IsHeld(ControllerButton button) const;
		bool IsPressed(ControllerButton button) const;
		bool IsReleased(ControllerButton button) const;

		void MapActionKey(ControllerKey key, std::unique_ptr<Command> action);

	private:
		const ControllerCommandMap& GetConsoleMap() const { return m_ConsoleCommands; }
		//Function for the unordered map
		static size_t ControllerButtonHash(const ControllerKey& button);

		ControllerCommandMap m_ConsoleCommands;

	};
}
