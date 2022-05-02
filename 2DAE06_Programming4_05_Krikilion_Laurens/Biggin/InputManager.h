#pragma once
#include <functional>
#include <unordered_map>
#include <variant>
#include "Singleton.h"
#include "iostream"

#pragma warning(push)
#pragma warning (disable:4201)
#include <glm/glm.hpp>
#pragma warning(pop)

class Command;

namespace biggin
{
	enum class ControllerButton
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
		None = 0
	};

	enum class ActionState
	{
		Up,
		Down,
		Hold,
		ThumbL,
		ThumbR,
		TriggerL,
		TriggerR
	};

	struct ActionKey
	{
		ActionState state{};
		ControllerButton button{};
		int controllerIdx{0};
		SDL_Keycode key{ SDLK_UNKNOWN };
		auto operator<=>(const ActionKey&) const = default;
	};
	//inline bool operator==(const ActionKey& x, const ActionKey& y)
	//{
	//	return std::tie(x.state, x.button, x.controllerIdx, x.key) == std::tie(y.state, y.button, y.controllerIdx, y.key);
	//}

	class InputManager final : public Singleton<InputManager>
	{
		struct InputContext
		{
			std::variant<float, glm::vec2> value{};
			ActionState state{};
			int controllerIdx{};

			template<typename T>
			T GetValue(){
				try{
					return std::get<T>(value);
				}
				catch (const std::bad_variant_access& ex){
					std::cerr << ex.what() << '\n';
				}
				return {};
			}
		};

		class InputManagerImpl;
		InputManagerImpl* m_pImpl;

		//using ControllerKey = std::pair<ButtonState, ControllerButton>;
		//https://stackoverflow.com/questions/2196995/is-there-any-advantage-of-using-map-over-unordered-map-in-case-of-trivial-keys
		using ControllerCommandMap = std::unordered_map<const ActionKey, std::unique_ptr<Command>, std::function<size_t(const ActionKey&)>>;

	public:
		InputManager();
		~InputManager() override;

		InputManager(const InputManager& other) = delete;
		InputManager(InputManager&& other) noexcept = delete;
		InputManager& operator=(const InputManager& other) = delete;
		InputManager& operator=(InputManager&& other) noexcept = delete;

		bool ProcessInput() const;
		bool IsHeld(ControllerButton button, int idx) const;
		bool IsPressed(ControllerButton button, int idx) const;
		bool IsReleased(ControllerButton button, int idx) const;
		glm::vec2 GetLThumb(int idx) const;
		glm::vec2 GetRThumb(int idx) const;
		float GetLTrigger(int idx) const;
		float GetRTrigger(int idx) const;

		InputContext GetInputContext() const;

		void MapActionKey(ActionKey key, std::unique_ptr<Command> action);

	private:
		void HandleInput(int i) const;

		const ControllerCommandMap& GetConsoleMap() const { return m_ConsoleCommands; }
		//Function for the unordered map
		static size_t ControllerButtonHash(const ActionKey& button);

		ControllerCommandMap m_ConsoleCommands;

	};
}
