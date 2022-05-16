#pragma once
#include "InputManager.h"

class Command
{
public:
	virtual ~Command() = default;
	virtual void execute() = 0;
};

//class JumpCommand final : public Command
//{
//public:
//	void execute() override { std::cout << "jump"; }
//};
//
//class DuckCommand final : public Command
//{
//public:
//	void execute() override { std::cout << "duck"; }
//};
//
//class FireCommand final : public Command
//{
//public:
//	void execute() override { std::cout << "pew pew"; }
//};
//
//class FartCommand final : public Command
//{
//public:
//	void execute() override { std::cout << "fart"; }
//};