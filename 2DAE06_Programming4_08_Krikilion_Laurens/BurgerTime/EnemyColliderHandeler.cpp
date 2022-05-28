#include "EnemyColliderHandeler.h"
#include "BoxCollider2d.h"
#include "GameObject.h"

EnemyColliderHandeler::EnemyColliderHandeler(biggin::GameObject* go)
	:Component(go)
{
}

void EnemyColliderHandeler::Initialize(biggin::GameObject* go)
{
	m_PlayerRef = go->GetSceneRef()->FindGameObjectsWithName("Player");
}

void EnemyColliderHandeler::OnNotify(Component* entity, const std::string& event)
{
	if (event == "BeginContact")
	{
		const auto tag = static_cast<const biggin::BoxCollider2d*>(entity)->GetOther()->GetTag();
		if (tag == "Player")
		{
			if (!m_Stunned && m_IsAlive)
			{
				const biggin::GameObject* playerGameObj = static_cast<const biggin::BoxCollider2d*>(entity)->GetOther()->GetOwningGameObject();
			}
		}
		else if (tag == "Burger")
		{
			
		}
		else if (tag == "Pepper")
		{
			m_Stunned = true;
		}
	}
}
