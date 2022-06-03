#include "EnemyColliderHandeler.h"
#include <iostream>
#include "BoxCollider2d.h"
#include "Burger.h"
#include "EnemyMovement.h"
#include "GameObject.h"
#include "Logger.h"
#include "PeterPepper.h"

EnemyColliderHandeler::EnemyColliderHandeler(biggin::GameObject* go, EnemyType type, const std::vector<Observer*>& observers)
	:Component(go)
	,m_EnemyType(type)
	,m_pNotifier(go->GetComponent<biggin::Subject>())
{
	if (m_pNotifier == nullptr)
		Logger::GetInstance().LogErrorAndBreak("Missing Subject Component");

	for (const auto observer : observers)
		m_pNotifier->AddObserver(observer);
}

EnemyColliderHandeler::~EnemyColliderHandeler()
{
	if (m_BurgerGameObjectRef != nullptr)
	{
		m_BurgerGameObjectRef->RemoveObserver(this);
		m_BurgerGameObjectRef = nullptr;
	}
}

void EnemyColliderHandeler::Initialize(biggin::GameObject* go)
{
	m_PlayerRef = go->GetSceneRef()->FindGameObjectsWithName("Player");
	m_MovementRef = go->GetComponent<EnemyMovement>();
}

void EnemyColliderHandeler::AddObservers(const std::vector<Observer*>& observers) const
{
	for (const auto observer : observers)
		m_pNotifier->AddObserver(observer);
}

void EnemyColliderHandeler::RemoveObservers(const std::vector<Observer*>& observers) const
{
	for (const auto observer : observers)
		m_pNotifier->RemoveObserver(observer);
}

void EnemyColliderHandeler::OnNotify(Component* entity, const std::string& event)
{
	if (!m_IsAlive)
		return;

	if (event == "BeginContact")
	{
		const auto* otherColider = static_cast<const biggin::BoxCollider2d*>(entity);
		const auto tag = otherColider->GetTag();

		if (tag == "Player")
		{
			HandleEnemyPlayerBeginContact(otherColider);
		}
		else if (tag == "Burger")
		{
			HandleEnemyBurgerBeginContact(otherColider);
		}
		else if (tag == "Pepper")
		{
			m_Stunned = true;
			m_MovementRef->Peppered();
		}
	}
	else if (event == "EndContact")
	{
		const auto* otherColider = static_cast<const biggin::BoxCollider2d*>(entity);
		const auto tag = otherColider->GetTag();

		if (tag == "Burger")
		{
			HandleEnemyBurgerEndContact(otherColider);
		}
	}
	else if (event == "BurgerFalling")
	{
		Die();
	}
}

void EnemyColliderHandeler::HandleEnemyPlayerBeginContact(const biggin::BoxCollider2d* otherColider)
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

void EnemyColliderHandeler::HandleEnemyBurgerBeginContact(const biggin::BoxCollider2d* otherColider)
{
	//the parent of the burger part were overlapping with is our actual burger game object
	const auto overlappedBurgerGameObject = otherColider->GetOwningGameObject()->GetParent();

	if (m_BurgerGameObjectRef == nullptr)
	{
		m_BurgerGameObjectRef = overlappedBurgerGameObject;
	}

	if(IsBurgerFalling(overlappedBurgerGameObject))
	{
		Die();
		return;
	}

	if (overlappedBurgerGameObject == m_BurgerGameObjectRef)
	{
		//as one burger is split up in multiple parts we keep track of how many part we are touching
		//so we know when we are touching no parts anymore and thus no burger anymore
		++m_AmntColliding;

		if (m_AmntColliding == 1) //first overlap
		{
			m_BurgerGameObjectRef->AddObserver(this);
		}
	}
}

void EnemyColliderHandeler::HandleEnemyBurgerEndContact(const biggin::BoxCollider2d* otherColider)
{
	if (otherColider->GetOwningGameObject()->GetParent() == m_BurgerGameObjectRef)
	{
		--m_AmntColliding;

		if (m_AmntColliding == 0)
		{
			m_BurgerGameObjectRef->RemoveObserver(this);
			m_BurgerGameObjectRef = nullptr;
		}
	}
}

bool EnemyColliderHandeler::IsBurgerFalling(biggin::GameObject* overlappedBurgerGameObject) const
{
	//if (overlappedBurgerGameObject != m_BurgerGameObjectRef || m_AmntColliding == 1)
	//{
		const auto burger = overlappedBurgerGameObject->GetComponent<Burger>();
		if (burger == nullptr)
			return false;

		return burger->IsFalling();
	//}
	//return false;
}

void EnemyColliderHandeler::Die()
{
	if (m_BurgerGameObjectRef != nullptr)
		GetGameObject()->RemoveComponentsPending<biggin::BoxCollider2d>();

	m_pNotifier->notify(this, "EnemyDied");
	m_IsAlive = false;
	m_MovementRef->Die();
}

void EnemyColliderHandeler::Update()
{
}

void EnemyColliderHandeler::FixedUpdate()
{
}
