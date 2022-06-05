#pragma once
enum class BurgerIngredients;
struct SDL_Window;

namespace biggin
{
	class Biggin
	{
	public:
		void Initialize();
		void Cleanup();
		void Run();

		static constexpr int GetWindowWidth() { return m_WindowWidth; }
		static constexpr int GetWindowHeight() { return m_WindowHeight; }

	private:
		SDL_Window* m_Window{};
		static constexpr int m_WindowWidth{ 912 };
		static constexpr int m_WindowHeight{ 700 };
	};
}