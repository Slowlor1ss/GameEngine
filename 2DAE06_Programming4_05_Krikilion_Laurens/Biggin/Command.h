#pragma once
#include <iostream>
#include "GameObject.h"
#include "InputManager.h"
#include "PeterPepper.h"
#include "ScoreComponent.h"

namespace character
{
	class PeterPepper;
}

class Command
{
public:
	virtual ~Command() = default;
	virtual void execute() = 0;
};

class JumpCommand final : public Command
{
public:
	void execute() override { std::cout << "jump"; }
};

class DuckCommand final : public Command
{
public:
	void execute() override { std::cout << "duck"; }
};

class FireCommand final : public Command
{
public:
	void execute() override { std::cout << "pew pew"; }
};

class FartCommand final : public Command
{
public:
	void execute() override { std::cout << "fart"; }
};

//TODO: spit up this file in mutiple files like debug commands
class DebugFloatCTXCommand final : public Command
{
public:
	void execute() override { std::cout << biggin::InputManager::GetInstance().GetInputContext().GetValue<float>() << '\n'; }
};

class DebugVec2CTXCommand final : public Command
{
public:
	void execute() override
	{
		const auto vec = biggin::InputManager::GetInstance().GetInputContext().GetValue<glm::vec2>();
		std::cout << "[" << vec.x << ", " << vec.y << "]\n";
	}
};

class PrintControls final : public Command
{
public:
	void execute() override
	{
		std::cout << "Controlls:\n"
			<< "\tDamage player one:\t\t[Controller button A]\n"
			<< "\tIncrease score player one:\t[Controller button X]\n"
			<< "\tDamage player two:\t\t[Controller button B]\n"
			<< "\tIncrease score player two:\t[Controller button Y]\n\n";
	}
};

class DamagePlayer final : public Command
{
public:
	DamagePlayer(character::PeterPepper* player) : m_Player(player) {}
	void execute() override { m_Player->Damage(); }
private:
	character::PeterPepper* m_Player;
};

//Just for example when enemies and other things that can increase score these will do this
class IncreaseScore final : public Command
{
public:
	IncreaseScore(biggin::GameObject* go) : m_Score(go->GetComponent<biggin::ScoreComponent>()) {}
	void execute() override { m_Score->IncreaseScore(100); }
private:
	biggin::ScoreComponent* m_Score;
};

class MoveCommand final : public Command
{
public:

	enum class ActionDirection { Left, Right, Up, Down, All };

	MoveCommand(character::PeterPepper* player, const ActionDirection direction)
	: m_Direction(direction)
	, m_Player(player)
	{
	}

	void execute() override {

		if (m_Player->IsDead())
			return;

		const float maxSpeed = m_Player->GetSpeed();

		switch (m_Direction)
		{
		case ActionDirection::Left:
			m_Player->SetVelocity({ -1 * maxSpeed,0 });
			break;
		case ActionDirection::Right:
			m_Player->SetVelocity({ 1 * maxSpeed,0 });
			break;
		case ActionDirection::Up:
			m_Player->SetVelocity({ 0,-1 * maxSpeed });
			break;
		case ActionDirection::Down:
			m_Player->SetVelocity({ 0,1 * maxSpeed });
			break;
		case ActionDirection::All:
			//negate y pos because (0,0) is at the top left
			auto velocity = biggin::InputManager::GetInstance().GetInputContext().GetValue<glm::vec2>()
													* glm::vec2{1,-1};
			velocity = velocity.x > velocity.y ? glm::vec2{ velocity.x, 0 } : glm::vec2{ 0, velocity.y };

			m_Player->SetVelocity(velocity*maxSpeed);
			break;
		}
	}

private:
	ActionDirection m_Direction;
	character::PeterPepper* m_Player;
};