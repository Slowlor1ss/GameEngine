#pragma once
#include <iostream>
#include "PeterPepper.h"

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

class DamagePlayer final : public Command
{
public:
	DamagePlayer(std::shared_ptr<character::PeterPepper> player) : m_Player(player) {}
	void execute() override { m_Player->Damage(); }
private:
	std::shared_ptr<character::PeterPepper> m_Player;
};
