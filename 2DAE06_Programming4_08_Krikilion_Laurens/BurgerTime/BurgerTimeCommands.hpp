#pragma once
#include "InputManager.h"
#include "PeterPepper.h"
#pragma warning(push, 0)
#include <glm/glm.hpp>
#pragma warning(pop)
#include <iostream>
#include "Command.h"
#include "ScoreComponent.h"
#include "GameObject.h"

class PrintControls final : public Command
{
public:
	void execute() override
	{
		std::cout << "Controlls:\n"
			<< "controller:\n"
			<< "\tMovement:\t[LThumbStick]\n"
			<< "\tPepper:\t\t[ButtonA]\n"
			<< "player one:\n"
			<< "\tMovement:\t[W,A,S,D]\n"
			<< "\tPepper:\t\t[Spacebar]\n"
			<< "player two:\n"
			<< "\tMovement:\t[Arrow Keys]\n"
			<< "\tPepper:\t\t[Enter]\n\n";
	}
};

class  MoveCommand final : public Command
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
				* glm::vec2{ 1,-1 };
			velocity = abs(velocity.x) > abs(velocity.y) ? glm::vec2{ velocity.x, 0 } : glm::vec2{ 0, velocity.y };

			m_Player->SetVelocity(velocity * maxSpeed);
			break;
		}
	}

private:
	ActionDirection m_Direction;
	character::PeterPepper* m_Player;
};

class  ShootCommand final : public Command
{
public:
	explicit ShootCommand(character::PeterPepper* player)
		: m_Player(player)
	{
	}

	void execute() override {
		m_Player->ShootPepper();
	}

private:
	character::PeterPepper* m_Player;
};

