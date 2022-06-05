#include "BigginPCH.h"
#include "EnemyMovement.h"
#include "BoxCollider2d.h"
#include "GameObject.h"
#include "GameTime.h"
#include "Logger.h"
#include "SpriteRenderComponent.h"
#pragma warning(push, 0)
#include <glm/glm.hpp>
#include "PeterPepper.h"
#pragma warning (pop)

using namespace enemy;

EnemyMovement::EnemyMovement(biggin::GameObject* go, float velocity)
	: Component(go)
	, m_CurrentDirection(movementDirection::none)
	, m_Velocity(velocity)
	, m_GameTimeRef{biggin::GameTime::GetInstance() }
{
	m_CooldownCounter = m_Cooldown;
}

void EnemyMovement::Initialize(biggin::GameObject* go)
{
	auto players = go->GetSceneRef()->FindGameObjectsWithName("Player");
	if (players.empty())
		Logger::GetInstance().LogErrorAndBreak("No Players Found!");

	int alivePlayerCount{};
	for (auto playerGO : players)
	{
		auto peter = playerGO->GetComponent<character::PeterPepper>();
		if (peter && !peter->IsDead())
		{
			++alivePlayerCount;
		}
	}


	m_PlayerRef = players[alivePlayerCount == 0 ? 0 : utils::randomInt(alivePlayerCount)];

	m_pSpriteComp = go->GetComponent<biggin::SpriteRenderComponent>();
	if (m_pSpriteComp == nullptr)
		Logger::GetInstance().LogErrorAndBreak("Missing SpriteRenderComponent");

}

void EnemyMovement::OnNotify(Component* entity, const std::string& event)
{
	if (m_Dead || m_Disabled)
		return;

	if (event == "BeginContact")
	{
		const auto pos = GetGameObject()->GetLocalPosition();
		const auto playerPos = m_PlayerRef->GetLocalPosition();

		const auto tag = static_cast<const biggin::BoxCollider2d*>(entity)->GetOther()->GetTag();
		if (tag == "ColliderTop")
		{
			++m_AmntCollidingTop;
			HandleHitWall(playerPos, pos, movementDirection::movingUp);
		}
		else if (tag == "ColliderBottom")
		{
			++m_AmntCollidingBottom;
			HandleHitWall(playerPos, pos, movementDirection::movingDown);
		}
		else if (tag == "ColliderLeft")
		{
			++m_AmntCollidingLeft;
			HandleHitWall(playerPos, pos, movementDirection::movingLeft);
		}
		else if (tag == "ColliderRight")
		{
			++m_AmntCollidingRight;
			HandleHitWall(playerPos, pos, movementDirection::movingRight);
		}
	}
	else if (event == "EndContact")
	{
		const auto tag = static_cast<const biggin::BoxCollider2d*>(entity)->GetOther()->GetTag();
		if (tag == "ColliderTop")
		{
			if (m_AmntCollidingTop>0)
				--m_AmntCollidingTop;
		}
		else if (tag == "ColliderBottom")
		{
			if (m_AmntCollidingBottom > 0)
				--m_AmntCollidingBottom;
		}
		else if (tag == "ColliderLeft")
		{
			if (m_AmntCollidingLeft > 0)
				--m_AmntCollidingLeft;
		}
		else if (tag == "ColliderRight")
		{
			if (m_AmntCollidingRight > 0)
				--m_AmntCollidingRight;
		}
	}
}

void EnemyMovement::HandleHitWall(const glm::vec2& playerPos, const glm::vec2& pos, movementDirection wallDir)
{
	if (CheckAndFixStuck())
		return;

	if (m_CurrentDirection == wallDir)
	{
		switch (wallDir)
		{
		case movementDirection::movingUp:
		case movementDirection::movingDown:
			if (utils::LessThanWithMargin(playerPos.x, pos.x, m_PlayerPosMargin) || m_AmntCollidingRight > 0)
			{
				m_CooldownCounter = m_Cooldown;
				SetDirection(movementDirection::movingLeft);
			}
			else
			{
				m_CooldownCounter = m_Cooldown;
				SetDirection(movementDirection::movingRight);
			}
			break;
		case movementDirection::movingLeft:
		case movementDirection::movingRight:
			if (utils::LessThanWithMargin(playerPos.y, pos.y, m_PlayerPosMargin) || m_AmntCollidingBottom > 0)
			{
				m_CooldownCounter = m_Cooldown;
				SetDirection(movementDirection::movingUp);
			}
			else
			{
				m_CooldownCounter = m_Cooldown;
				SetDirection(movementDirection::movingDown);
			}
			break;
		}
	}

}

void EnemyMovement::SetDirection( movementDirection newDir)
{
	switch (newDir)
	{
	case movementDirection::movingUp:
		m_CurrentDirection = movementDirection::movingUp;
		m_pSpriteComp->SetPause(false); //just in case
		m_pSpriteComp->SetCurrentSprite(static_cast<int>(AnimationState::runVertical));
		break;
	case movementDirection::movingDown:
		m_CurrentDirection = movementDirection::movingDown;
		m_pSpriteComp->SetPause(false); //just in case
		m_pSpriteComp->SetCurrentSprite(static_cast<int>(AnimationState::runVertical));
		break;
	case movementDirection::movingLeft:
		m_CurrentDirection = movementDirection::movingLeft;
		m_pSpriteComp->SetPause(false); //just in case
		m_pSpriteComp->SetCurrentSprite(static_cast<int>(AnimationState::runHorizontal));
		m_pSpriteComp->SetFlip(SDL_FLIP_NONE);
		break;
	case movementDirection::movingRight:
		m_CurrentDirection = movementDirection::movingRight;
		m_pSpriteComp->SetPause(false); //just in case
		m_pSpriteComp->SetCurrentSprite(static_cast<int>(AnimationState::runHorizontal));
		m_pSpriteComp->SetFlip(SDL_FLIP_HORIZONTAL);
		break;
	default: ;
	}
}

void EnemyMovement::Update()
{
	if (m_Dead)
		return;

	if (m_Disabled)
	{
		m_DelayedResetDisabled.Update(m_GameTimeRef.GetDeltaT());
		return;
	}

	if (m_CooldownCounter > 0)
	{
		m_CooldownCounter -= m_GameTimeRef.GetDeltaT();
		return;
	}

	const auto pos = GetGameObject()->GetLocalPosition();
	const auto playerPos = m_PlayerRef->GetLocalPosition();

	if (IsMovingSideways())
	{
		if (m_AmntCollidingTop == 0 && utils::LessThanWithMargin(playerPos.y, pos.y, m_PlayerPosMargin) ) //check if we can move up and if player is above us
		{
			m_CooldownCounter = m_Cooldown;

			SetDirection(movementDirection::movingUp);
		}
		else if (m_AmntCollidingBottom == 0 && utils::GreaterThanWithMargin(playerPos.y, pos.y, m_PlayerPosMargin) ) //check if we can move down and if player is below us
		{
			m_CooldownCounter = m_Cooldown;

			SetDirection(movementDirection::movingDown);
		}
	}
	else //is moving vertical
	{
		if (m_AmntCollidingLeft == 0 && utils::LessThanWithMargin(playerPos.x, pos.x, m_PlayerPosMargin) )
		{
			m_CooldownCounter = m_Cooldown;

			SetDirection(movementDirection::movingLeft);
		}
		else if (m_AmntCollidingRight == 0 && utils::GreaterThanWithMargin(playerPos.x, pos.x, m_PlayerPosMargin) )
		{
			m_CooldownCounter = m_Cooldown;

			SetDirection(movementDirection::movingRight);
		}
	}
}

void EnemyMovement::FixedUpdate()
{
	if(m_Disabled || m_Dead)
		return;

	switch (m_CurrentDirection)
	{
	case movementDirection::movingUp:
		GetGameObject()->TranslateLocalPosition(glm::vec2{ 0,-1 } * m_Velocity * biggin::GameTime::GetFixedTimeStep());
		break;
	case movementDirection::movingDown:
		GetGameObject()->TranslateLocalPosition(glm::vec2{ 0,1 } * m_Velocity * biggin::GameTime::GetFixedTimeStep());
		break;
	case movementDirection::movingLeft:
		GetGameObject()->TranslateLocalPosition(glm::vec2{ -1,0 } * m_Velocity * biggin::GameTime::GetFixedTimeStep());
		break;
	case movementDirection::movingRight:
		GetGameObject()->TranslateLocalPosition(glm::vec2{ 1,0 } * m_Velocity * biggin::GameTime::GetFixedTimeStep());
		break;
	}
}

void EnemyMovement::Peppered(float time)
{
	m_DelayedResetDisabled.func = [this] {m_Disabled = false;};
	m_DelayedResetDisabled.interval = time;
	m_DelayedResetDisabled.Reset();
	m_Disabled = true;
}

void EnemyMovement::Die()
{
	m_Dead = true;
}

bool EnemyMovement::CheckAndFixStuck()
{
	switch (m_CurrentDirection)
	{
	case movementDirection::movingUp:
		if (m_AmntCollidingTop > 0 && m_AmntCollidingLeft > 0 && m_AmntCollidingRight > 0)
		{
			m_CooldownCounter = m_Cooldown;

			SetDirection(movementDirection::movingDown);
			return true;
		}
		break;
	case movementDirection::movingDown:
		if (m_AmntCollidingBottom > 0 && m_AmntCollidingLeft > 0 && m_AmntCollidingRight > 0)
		{
			m_CooldownCounter = m_Cooldown;

			SetDirection(movementDirection::movingUp);
			return true;
		}
		break;
	case movementDirection::movingLeft:
		if (m_AmntCollidingLeft > 0 && m_AmntCollidingTop > 0 && m_AmntCollidingBottom > 0)
		{
			m_CooldownCounter = m_Cooldown;

			SetDirection(movementDirection::movingRight);
			return true;
		}
		break;
	case movementDirection::movingRight:
		if (m_AmntCollidingRight > 0 && m_AmntCollidingTop > 0 && m_AmntCollidingBottom > 0)
		{
			m_CooldownCounter = m_Cooldown;

			SetDirection(movementDirection::movingLeft);
			return true;
		}
		break;
	}
	return false;
}
