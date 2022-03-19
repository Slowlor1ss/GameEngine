#pragma once
#include <iostream>
#include "GameObject.h"
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
