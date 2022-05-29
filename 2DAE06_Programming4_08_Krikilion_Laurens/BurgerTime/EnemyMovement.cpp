#include "BigginPCH.h"
#include "EnemyMovement.h"
#include "BoxCollider2d.h"
#include "GameObject.h"
#include "GameTime.h"
#include "Logger.h"
#include "SpriteRenderComponent.h"

EnemyMovement::EnemyMovement(biggin::GameObject* go, movementDirection movDir)
	:Component(go)
	,m_CurrentDirection(movDir)
	,m_GameTimeRef{ GameTime::GetInstance() }
{
	//Interval is set in disabled function
	m_DelayedResetDisabled = utils::DelayedCallback(0, [&] {m_Disabled = false; }, 1);

	m_CooldownCounter = m_Cooldown;
}

void EnemyMovement::Initialize(biggin::GameObject* go)
{
	m_PlayerRef = go->GetSceneRef()->FindGameObjectWithName("Player");

	m_pSpriteComp = go->GetComponent<biggin::SpriteRenderComponent>();
	if (m_pSpriteComp == nullptr)
		Logger::GetInstance().LogErrorAndBreak("Missing SpriteRenderComponent");

	m_pSpriteComp->SetCurrentSprite(static_cast<int>(AnimationState::runHorizontal));
	if (m_CurrentDirection == movementDirection::movingRight)
		m_pSpriteComp->SetFlip(SDL_FLIP_HORIZONTAL);
}

void EnemyMovement::OnNotify(Component* entity, const std::string& event)
{
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
			--m_AmntCollidingTop;
		}
		else if (tag == "ColliderBottom")
		{
			--m_AmntCollidingBottom;
		}
		else if (tag == "ColliderLeft")
		{
			--m_AmntCollidingLeft;
		}
		else if (tag == "ColliderRight")
		{
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
			if (utils::LessThanWithMargin(playerPos.x, pos.x, m_PlayerPosMargin))
			{
				m_CurrentDirection = movementDirection::movingLeft;
				m_CooldownCounter = m_Cooldown;
			}
			else
			{
				m_CurrentDirection = movementDirection::movingRight;
				m_CooldownCounter = m_Cooldown;
			}
			break;
		case movementDirection::movingLeft:
		case movementDirection::movingRight:
			if (utils::LessThanWithMargin(playerPos.y, pos.y, m_PlayerPosMargin))
			{
				m_CurrentDirection = movementDirection::movingUp;
				m_CooldownCounter = m_Cooldown;
			}
			else
			{
				m_CurrentDirection = movementDirection::movingDown;
				m_CooldownCounter = m_Cooldown;
			}
			break;
		}
	}

}

void EnemyMovement::Update()
{
	if (m_Disabled)
	{
		m_DelayedResetDisabled.Update(m_GameTimeRef.GetDeltaT());
		return;
	}

	if (m_CooldownCounter > 0)
	{
		m_CooldownCounter -= m_GameTimeRef.GetDeltaT();
		if (m_CooldownCounter <5 && m_CooldownCounter > 4)
		{
			std::cout << m_CooldownCounter << '\n';
		}
		return;
	}

	const auto pos = GetGameObject()->GetLocalPosition();
	const auto playerPos = m_PlayerRef->GetLocalPosition();

	if (IsMovingSideways())
	{
		if (m_AmntCollidingTop == 0 && utils::LessThanWithMargin(playerPos.y, pos.y, m_PlayerPosMargin) ) //check if we can move up and if player is above us
		{
			m_CooldownCounter = m_Cooldown;

			m_CurrentDirection = movementDirection::movingUp;
			std::cout << "moving up\n";
			m_pSpriteComp->SetPause(false); //just in case
			m_pSpriteComp->SetCurrentSprite(static_cast<int>(AnimationState::runVertical));
		}
		else if (m_AmntCollidingBottom == 0 && utils::GreaterThanWithMargin(playerPos.y, pos.y, m_PlayerPosMargin) ) //check if we can move down and if player is below us
		{
			m_CooldownCounter = m_Cooldown;

			m_CurrentDirection = movementDirection::movingDown;
			std::cout << "moving down\n";
			m_pSpriteComp->SetPause(false); //just in case
			m_pSpriteComp->SetCurrentSprite(static_cast<int>(AnimationState::runVertical));
		}
	}
	else //is moving vertical
	{
		if (m_AmntCollidingLeft == 0 && utils::LessThanWithMargin(playerPos.x, pos.x, m_PlayerPosMargin) )
		{
			m_CooldownCounter = m_Cooldown;

			m_CurrentDirection = movementDirection::movingLeft;
			std::cout << "moving left\n";
			m_pSpriteComp->SetPause(false); //just in case
			m_pSpriteComp->SetCurrentSprite(static_cast<int>(AnimationState::runHorizontal));
			m_pSpriteComp->SetFlip(SDL_FLIP_NONE);
		}
		else if (m_AmntCollidingRight == 0 && utils::GreaterThanWithMargin(playerPos.x, pos.x, m_PlayerPosMargin) )
		{
			m_CooldownCounter = m_Cooldown;

			m_CurrentDirection = movementDirection::movingRight;
			std::cout << "moving right\n";
			m_pSpriteComp->SetPause(false); //just in case
			m_pSpriteComp->SetCurrentSprite(static_cast<int>(AnimationState::runHorizontal));
			m_pSpriteComp->SetFlip(SDL_FLIP_HORIZONTAL);
		}
	}
}

void EnemyMovement::FixedUpdate()
{
	//if(m_Disabled)
	//	return;

	//switch (m_CurrentDirection)
	//{
	//case movementDirection::movingUp:
	//	GetGameObject()->TranslateLocalPosition(glm::vec2{ 0,-1 } * m_Velocity * GameTime::GetFixedTimeStep());
	//	break;
	//case movementDirection::movingDown:
	//	GetGameObject()->TranslateLocalPosition(glm::vec2{ 0,1 }
	//	* m_Velocity * GameTime::GetFixedTimeStep());
	//	break;
	//case movementDirection::movingLeft:
	//	GetGameObject()->TranslateLocalPosition(glm::vec2{ -1,0 } * m_Velocity * GameTime::GetFixedTimeStep());
	//	break;
	//case movementDirection::movingRight:
	//	GetGameObject()->TranslateLocalPosition(glm::vec2{ 1,0 } * m_Velocity * GameTime::GetFixedTimeStep());
	//	break;
	//}
}

//void EnemyMovement::Disable()
//{
//	m_Disabled = true;
//	m_pSpriteComp->SetPause(true);
//}

void EnemyMovement::Peppered(float time)
{
	m_DelayedResetDisabled.interval = time;
	m_DelayedResetDisabled.Reset();
	m_Disabled = true;
	m_pSpriteComp->SetCurrentSprite(static_cast<int>(AnimationState::peppered));
	m_pSpriteComp->SetFinishAndPause();
}

void EnemyMovement::Die()
{
	m_Disabled = true;
	m_pSpriteComp->SetCurrentSprite(static_cast<int>(AnimationState::die));
	m_pSpriteComp->SetFinishAndPause();
}

bool EnemyMovement::CheckAndFixStuck()
{
	switch (m_CurrentDirection)
	{
	case movementDirection::movingUp:
		if (m_AmntCollidingTop > 0 && m_AmntCollidingLeft > 0 && m_AmntCollidingRight > 0)
		{
			m_CooldownCounter = m_Cooldown;

			m_CurrentDirection = movementDirection::movingDown;
			return true;
		}
		break;
	case movementDirection::movingDown:
		if (m_AmntCollidingBottom > 0 && m_AmntCollidingLeft > 0 && m_AmntCollidingRight > 0)
		{
			m_CooldownCounter = m_Cooldown;

			m_CurrentDirection = movementDirection::movingUp;
			return true;
		}
		break;
	case movementDirection::movingLeft:
		if (m_AmntCollidingLeft > 0 && m_AmntCollidingTop > 0 && m_AmntCollidingBottom > 0)
		{
			m_CooldownCounter = m_Cooldown;

			m_CurrentDirection = movementDirection::movingRight;
			return true;
		}
		break;
	case movementDirection::movingRight:
		if (m_AmntCollidingRight > 0 && m_AmntCollidingTop > 0 && m_AmntCollidingBottom > 0)
		{
			m_CooldownCounter = m_Cooldown;

			m_CurrentDirection = movementDirection::movingLeft;
			return true;
		}
		break;
	}
	return false;
}

//TODO:clean up

//bool EnemyMovement::FixStuckMovingUp()
//{
//	if (m_AmntCollidingLeft == 0)
//	{
//		m_CurrentDirection = movementDirection::movingLeft;
//		return true;
//	}
//	else if (m_AmntCollidingRight == 0)
//	{
//		m_CurrentDirection = movementDirection::movingRight;
//		return true;
//	}
//	else if (m_AmntCollidingBottom == 0)
//	{
//		m_CurrentDirection = movementDirection::movingDown;
//		return true;
//	}
//	return false; //stuck beyond repair :X
//}
//
//bool EnemyMovement::FixStuckMovingDown()
//{
//	if (m_AmntCollidingLeft == 0)
//	{
//		m_CurrentDirection = movementDirection::movingLeft;
//		return true;
//	}
//	else if (m_AmntCollidingRight == 0)
//	{
//		m_CurrentDirection = movementDirection::movingRight;
//		return true;
//	}
//	else if (m_AmntCollidingTop == 0)
//	{
//		m_CurrentDirection = movementDirection::movingUp;
//		return true;
//	}
//	return false; //stuck beyond repair :X
//}
//
//bool EnemyMovement::FixStuckMovingLeft()
//{
//	if (m_AmntCollidingTop == 0)
//	{
//		m_CurrentDirection = movementDirection::movingUp;
//		return true;
//	}
//	else if (m_AmntCollidingBottom == 0)
//	{
//		m_CurrentDirection = movementDirection::movingDown;
//		return true;
//	}
//	else if (m_AmntCollidingRight == 0)
//	{
//		m_CurrentDirection = movementDirection::movingRight;
//		return true;
//	}
//	return false; //stuck beyond repair :X
//}
//
//bool EnemyMovement::FixStuckMovingRight()
//{
//	if (m_AmntCollidingTop == 0)
//	{
//		m_CurrentDirection = movementDirection::movingUp;
//		return true;
//	}
//	else if (m_AmntCollidingBottom == 0)
//	{
//		m_CurrentDirection = movementDirection::movingDown;
//		return true;
//	}
//	else if (m_AmntCollidingLeft == 0)
//	{
//		m_CurrentDirection = movementDirection::movingLeft;
//		return true;
//	}
//	return false; //stuck beyond repair :X
//}
