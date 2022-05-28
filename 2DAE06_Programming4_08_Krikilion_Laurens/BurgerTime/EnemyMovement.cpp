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
	//Reset old pos after interval
	m_DelayedResetOldDir = utils::DelayedCallback(0.2f, [&] {m_OldDirection = movementDirection::none; }, 1);
	//Interval is set in disabled function
	m_DelayedResetDisabled = utils::DelayedCallback(0, [&] {m_Disabled = false; }, 1);
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
		const auto tag = static_cast<const biggin::BoxCollider2d*>(entity)->GetOther()->GetTag();
		if (tag == "ColliderTop")
		{
			++m_AmntCollidingTop;
		}
		else if (tag == "ColliderBottom")
		{
			++m_AmntCollidingBottom;
		}
		else if (tag == "ColliderLeft")
		{
			++m_AmntCollidingLeft;
		}
		else if (tag == "ColliderRight")
		{
			++m_AmntCollidingRight;
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

void EnemyMovement::Update()
{
	if (m_Disabled)
	{
		m_DelayedResetDisabled.Update(m_GameTimeRef.GetDeltaT());
		return;
	}

	m_DelayedResetOldDir.Update(m_GameTimeRef.GetDeltaT());

	//check if stuck
	if(CheckAndFixStuck())
		return;

	const auto pos = GetGameObject()->GetLocalPosition();
	const auto playerPos = m_PlayerRef->GetLocalPosition();

	if (IsMovingSideways())
	{
		if (m_AmntCollidingTop == 0 && utils::LessThanWithMargin(playerPos.y, pos.y, m_PlayerPosMargin) 
			&& m_OldDirection != movementDirection::movingDown) //check if we can move up and if player is above us
		{
			m_OldDirection = m_CurrentDirection;
			m_CurrentDirection = movementDirection::movingUp;
			m_pSpriteComp->SetPause(false); //just in case
			m_pSpriteComp->SetCurrentSprite(static_cast<int>(AnimationState::runVertical));
			m_DelayedResetOldDir.Reset();
		}
		else if (m_AmntCollidingBottom == 0 && utils::GreaterThanWithMargin(playerPos.y, pos.y, m_PlayerPosMargin) 
			&& m_OldDirection != movementDirection::movingUp) //check if we can move down and if player is below us
		{
			m_OldDirection = m_CurrentDirection;
			m_CurrentDirection = movementDirection::movingDown;
			m_pSpriteComp->SetPause(false); //just in case
			m_pSpriteComp->SetCurrentSprite(static_cast<int>(AnimationState::runVertical));
			m_DelayedResetOldDir.Reset();
		}
	}
	else //is moving vertical
	{
		if (m_AmntCollidingLeft == 0 && utils::LessThanWithMargin(playerPos.x, pos.x, m_PlayerPosMargin) 
			&& m_OldDirection != movementDirection::movingRight)
		{
			m_OldDirection = m_CurrentDirection;
			m_CurrentDirection = movementDirection::movingLeft;
			m_pSpriteComp->SetPause(false); //just in case
			m_pSpriteComp->SetCurrentSprite(static_cast<int>(AnimationState::runHorizontal));
			m_pSpriteComp->SetFlip(SDL_FLIP_NONE);
			m_DelayedResetOldDir.Reset();
		}
		else if (m_AmntCollidingRight == 0 && utils::GreaterThanWithMargin(playerPos.x, pos.x, m_PlayerPosMargin) 
			&& m_OldDirection != movementDirection::movingLeft)
		{
			m_OldDirection = m_CurrentDirection;
			m_CurrentDirection = movementDirection::movingRight;
			m_pSpriteComp->SetPause(false); //just in case
			m_pSpriteComp->SetCurrentSprite(static_cast<int>(AnimationState::runHorizontal));
			m_pSpriteComp->SetFlip(SDL_FLIP_HORIZONTAL);
			m_DelayedResetOldDir.Reset();
		}
	}

}

void EnemyMovement::FixedUpdate()
{
	if(m_Disabled)
		return;

	switch (m_CurrentDirection)
	{
	case movementDirection::movingUp:
		GetGameObject()->TranslateLocalPosition(glm::vec2{ 0,-1 } * m_Velocity * GameTime::GetFixedTimeStep());
		break;
	case movementDirection::movingDown:
		GetGameObject()->TranslateLocalPosition(glm::vec2{ 0,1 }
		* m_Velocity * GameTime::GetFixedTimeStep());
		break;
	case movementDirection::movingLeft:
		GetGameObject()->TranslateLocalPosition(glm::vec2{ -1,0 } * m_Velocity * GameTime::GetFixedTimeStep());
		break;
	case movementDirection::movingRight:
		GetGameObject()->TranslateLocalPosition(glm::vec2{ 1,0 } * m_Velocity * GameTime::GetFixedTimeStep());
		break;
	}
}

void EnemyMovement::Disable()
{
	m_Disabled = true;
	m_pSpriteComp->SetPause(true);
}

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
		if (m_AmntCollidingTop > 0)
		{
			FixStuckMovingUp();
			return true;
		}
		break;
	case movementDirection::movingDown:
		if (m_AmntCollidingBottom > 0)
		{
			FixStuckMovingDown();
			return true;
		}
		break;
	case movementDirection::movingLeft:
		if (m_AmntCollidingLeft > 0)
		{
			FixStuckMovingLeft();
			return true;
		}
		break;
	case movementDirection::movingRight:
		if (m_AmntCollidingRight > 0)
		{
			FixStuckMovingRight();
			return true;
		}
		break;
	}
	return false;
}

bool EnemyMovement::FixStuckMovingUp()
{
	if (m_AmntCollidingLeft == 0)
	{
		m_CurrentDirection = movementDirection::movingLeft;
		return true;
	}
	else if (m_AmntCollidingRight == 0)
	{
		m_CurrentDirection = movementDirection::movingRight;
		return true;
	}
	else if (m_AmntCollidingBottom == 0)
	{
		m_CurrentDirection = movementDirection::movingDown;
		return true;
	}
	return false; //stuck beyond repair :X
}

bool EnemyMovement::FixStuckMovingDown()
{
	if (m_AmntCollidingLeft == 0)
	{
		m_CurrentDirection = movementDirection::movingLeft;
		return true;
	}
	else if (m_AmntCollidingRight == 0)
	{
		m_CurrentDirection = movementDirection::movingRight;
		return true;
	}
	else if (m_AmntCollidingTop == 0)
	{
		m_CurrentDirection = movementDirection::movingUp;
		return true;
	}
	return false; //stuck beyond repair :X
}

bool EnemyMovement::FixStuckMovingLeft()
{
	if (m_AmntCollidingTop == 0)
	{
		m_CurrentDirection = movementDirection::movingUp;
		return true;
	}
	else if (m_AmntCollidingBottom == 0)
	{
		m_CurrentDirection = movementDirection::movingDown;
		return true;
	}
	else if (m_AmntCollidingRight == 0)
	{
		m_CurrentDirection = movementDirection::movingRight;
		return true;
	}
	return false; //stuck beyond repair :X
}

bool EnemyMovement::FixStuckMovingRight()
{
	if (m_AmntCollidingTop == 0)
	{
		m_CurrentDirection = movementDirection::movingUp;
		return true;
	}
	else if (m_AmntCollidingBottom == 0)
	{
		m_CurrentDirection = movementDirection::movingDown;
		return true;
	}
	else if (m_AmntCollidingLeft == 0)
	{
		m_CurrentDirection = movementDirection::movingLeft;
		return true;
	}
	return false; //stuck beyond repair :X
}
