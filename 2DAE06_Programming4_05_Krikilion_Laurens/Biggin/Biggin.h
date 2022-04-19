#pragma once
enum class BurgerIngredients;
struct SDL_Window;
namespace biggin
{
	class Biggin
	{
	public:
		void Initialize();
		void LoadGame() const;
		void Cleanup();
		void Run();

		static void BurgerPrefab(BurgerIngredients ingredient);

	private:
		static constexpr int MsPerFrame = 16; //16 for 60 fps, 33 for 30 fps //TODO: check if this is used
		SDL_Window* m_Window{};
	};
}