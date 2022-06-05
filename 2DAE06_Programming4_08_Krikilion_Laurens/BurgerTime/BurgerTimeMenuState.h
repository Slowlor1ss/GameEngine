#pragma once
#include <string>
#include "Observer.h"

//https://gameprogrammingpatterns.com/state.html

class HighScoreState;
class ConfirmationState;
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

	virtual void RenderMenu(BurgerTimeMenuState*& currState) = 0;

	virtual void Enter() = 0;
	virtual void Exit() = 0;

	static void Cleanup();
	static void Start(BurgerTimeMenuState* startState) { startState->Enter(); m_pPrevState = startState; }

	//TODO: make getter functions
	static MainMenuState* GetMainMenu() { return m_pMainMenuState; }

protected:
	static void ChangeState(BurgerTimeMenuState*& currState, BurgerTimeMenuState* nextState);

	static MainMenuState* m_pMainMenuState;
	static RunningState* m_pRunningState;
	static OptionsState* m_pOptionsState;
	static ConfirmationState* m_pConfirmationState;
	static HighScoreState* m_pHighScoreState;

	static BurgerTimeMenuState* m_pPrevState;
	inline static bool m_IsOpen{true};
};

class MainMenuState final : public BurgerTimeMenuState
{
public:
	~MainMenuState() override = default;

	void RenderMenu(BurgerTimeMenuState*& currState) override;

private:
	void LoadSinglePlayer();
	void LoadCoop();
	void LoadVersus();

protected:
	void Enter() override;
	void Exit() override {}

};

class RunningState final : public BurgerTimeMenuState, public biggin::Observer
{
public:
	~RunningState() override = default;

	void OnNotify(biggin::Component* entity, const std::string& event) override;

	void RenderMenu(BurgerTimeMenuState*& currState) override;

protected:
	void Enter() override;
	void Exit() override {}

private:
	bool m_GameOver{false};

};

class OptionsState final : public BurgerTimeMenuState
{
public:
	~OptionsState() override = default;

	void RenderMenu(BurgerTimeMenuState*& currState) override;

protected:
	void Enter() override;
	void Exit() override;

};

class ConfirmationState final : public BurgerTimeMenuState
{
public:
	~ConfirmationState() override = default;

	void RenderMenu(BurgerTimeMenuState*& currState) override;

protected:
	void Enter() override;
	void Exit() override;

};

class HighScoreState final : public BurgerTimeMenuState
{
public:
	~HighScoreState() override = default;

	void RenderMenu(BurgerTimeMenuState*& currState) override;

protected:
	void Enter() override;
	void Exit() override;

private:
	void LoadHighScoreScene();

	float m_ElapsedSec{};

};
