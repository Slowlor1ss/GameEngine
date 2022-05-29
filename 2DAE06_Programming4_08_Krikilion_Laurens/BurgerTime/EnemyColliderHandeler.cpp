#include "EnemyColliderHandeler.h"
#include <iostream>
#include "BoxCollider2d.h"
#include "EnemyMovement.h"
#include "GameObject.h"
#include "PeterPepper.h"

EnemyColliderHandeler::EnemyColliderHandeler(biggin::GameObject* go)
	:Component(go)
{
}

void EnemyColliderHandeler::Initialize(biggin::GameObject* go)
{
	m_PlayerRef = go->GetSceneRef()->FindGameObjectsWithName("Player");
	m_MovementRef = go->GetComponent<EnemyMovement>();
}

void EnemyColliderHandeler::OnNotify(Component* entity, const std::string& event)
{
	if (event == "BeginContact")
	{
		const auto* otherColider = static_cast<const biggin::BoxCollider2d*>(entity);
		const auto tag = otherColider->GetTag();
		std::cout << tag << '\n';
		if (tag == "Player")
		{
			if (!m_Stunned && m_IsAlive)
			{
				const biggin::GameObject* playerGameObj = otherColider->GetOwningGameObject();
				const auto it = std::ranges::find_if(m_PlayerRef,
				                                     [playerGameObj](const std::shared_ptr<biggin::GameObject>& object)
				                                     {
					                                     return object.get() == playerGameObj;
				                                     });
				if (it == m_PlayerRef.end())	
					return;

				(*it).get()->GetComponent<character::PeterPepper>()->Damage();
			}
		}
		else if (tag == "Burger")
		{
			
		}
		else if (tag == "Pepper")
		{
			m_Stunned = true;
			m_MovementRef->Peppered();
		}
	}
	else if (event == "EndContact")
	{
		const auto tag = static_cast<const biggin::BoxCollider2d*>(entity)->GetOther()->GetTag();
		if (tag == "Burger")
		{

		}
	}
	else if (event == "BurgerFalling")
	{
		m_IsAlive = false;
		m_MovementRef->Die();
	}
}

void EnemyColliderHandeler::Update()
{
}

void EnemyColliderHandeler::FixedUpdate()
{
}
