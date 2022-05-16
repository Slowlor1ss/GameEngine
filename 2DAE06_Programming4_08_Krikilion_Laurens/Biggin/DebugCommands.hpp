#pragma once
#include "BigginPCH.h"
#include "Command.h"
#include "InputManager.h"
#include "Singleton.h"


class DebugFloatCTXCommand final : public Command
{
public:
	void execute() override{
		std::cout << biggin::InputManager::GetInstance().GetInputContext().GetValue<float>() << '\n';
	}
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