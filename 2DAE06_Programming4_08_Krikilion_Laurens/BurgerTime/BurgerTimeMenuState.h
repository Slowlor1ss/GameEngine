#pragma once
#include <string>
#include "MenuState.h"
#include "Observer.h"

//https://gameprogrammingpatterns.com/state.html

class GameLoader;
class RunningState;
class OptionsState;
class MainMenuState;

namespace biggin
{
	class Component;
}

class BurgerTimeMenuState
{
public:
	BurgerTimeMenuState() = default;
	virtual ~BurgerTimeMenuState() = default;

	BurgerTimeMenuState(const BurgerTimeMenuState& other) = delete;
	BurgerTimeMenuState(BurgerTimeMenuState&& other) noexcept = delete;
	BurgerTimeMenuState& operator=(const BurgerTimeMenuState& other) = delete;
	BurgerTimeMenuState& operator=(BurgerTimeMenuState&& other) noexcept = delete;

	virtual void RenderMenu(GameLoader* state) = 0;

	virtual void Enter() = 0;
	virtual void Exit() = 0;

	static void Cleanup();

	//TODO: make getter functions
	static MainMenuState* GetMainMenu() { return m_pMainMenuState; }

protected:
	static MainMenuState* m_pMainMenuState;
	static RunningState* m_pRunningState;
	static OptionsState* m_pOptionsState;
	inline static bool m_IsOpen{true};
};

class MainMenuState final : public BurgerTimeMenuState
{
public:
	~MainMenuState() override = default;

	void RenderMenu(GameLoader* UiMenu) override;

private:
	void LoadSinglePlayer();

	bool m_IsLoadedSinglePlayer{ false };
	bool m_IsLoadedMultiPlayer{ false };

protected:
	void Enter() override;
	void Exit() override {}

};

class RunningState final : public BurgerTimeMenuState, public biggin::Observer
{
public:
	~RunningState() override = default;

	void OnNotify(biggin::Component* entity, const std::string& event) override;

	void RenderMenu(GameLoader* UiMenu) override;

protected:
	void Enter() override;
	void Exit() override {}

};

class OptionsState final : public BurgerTimeMenuState
{
public:
	~OptionsState() override = default;

	void RenderMenu(GameLoader* UiMenu) override;

protected:
	void Enter() override;
	void Exit() override {}

};
