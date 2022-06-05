#include "BigginPCH.h"
#include "GameObject.h"
#include "Logger.h"
#include "Utils.hpp"
#include "Transform.h"
#include "Component.h"
#include "Scene.h"
#include "Subject.h"

using namespace biggin;

GameObject::GameObject() : m_pTransform(new Transform(this))
{
	m_Components.emplace_back(m_pTransform);
}

biggin::GameObject::GameObject(GameObject* parent) : m_pTransform(new Transform(this))
{
	m_Components.emplace_back(m_pTransform);

	SetParent(parent);
}

biggin::GameObject::~GameObject()
{
	//I think it would be logical to also delete the childeren when the parent gets deleted but this gives a weird crash and I realy really don't understand it
	//for (unsigned i{ 0 }; i < m_Childeren.size(); ++i)
	//{
	//	m_SceneRef->Remove(m_Childeren[i]);
	////	//RemoveChild(i);
	////	delete m_Childeren[i];
	////	m_Childeren[i] = nullptr;
	//}

	for (auto* pComponent : m_Components)
	{
		delete pComponent;
		pComponent = nullptr;
	}

}

void biggin::GameObject::Initialize(GameObject* go)
{
	for (auto* pComponent : m_Components)
	{
		pComponent->Initialize(go);
	}
}

void biggin::GameObject::Start()
{
	for (auto* pComponent : m_Components)
	{
		pComponent->Start();
	}
}

void GameObject::Update()
{
	for (auto* pComponent : m_Components)
	{
		pComponent->Update();
	}

	for (auto& component : m_pPendingAdd)
	{
		m_Components.emplace_back(component);
		//component->Initialize(this);
	}
	m_pPendingAdd.clear();

	for (auto& component : m_pPendingDelete)
	{
		delete component;
		m_Components.erase(std::find(m_Components.begin(), m_Components.end(), component));
	}
	m_pPendingDelete.clear();
}

void GameObject::FixedUpdate()
{
	for (auto* pComponent : m_Components)
	{
		pComponent->FixedUpdate();
	}
}

void GameObject::Render() const
{
	for (const auto* pComponent : m_Components)
	{
		pComponent->Render();
	}
}

void GameObject::RenderDebug()
{
	for (auto* pComponent : m_Components)
	{
		pComponent->RenderDebug();
	}
}

void GameObject::SetParent(GameObject* parent, bool keepWorldPos)
{
	//Check if given parent isn't already the parent
	if (&m_Parent == &parent)
	{
#ifdef _DEBUG
		Logger::GetInstance().LogWarning("SetParent called with current parent as parameter.");
#endif
		return;
	}

	const auto& oldParent = m_Parent;
	if (!oldParent)
		SetLocalPosition(GetWorldPosition());
	else
	{
		if (keepWorldPos)
			SetLocalPosition(GetLocalPosition() - oldParent->GetWorldPosition());
		m_PositionDirty = true;
	}

	//Check if there already was a parent and if so remove itself as a child
	if (oldParent)
		oldParent->RemoveChild(this);

	m_Parent = parent;

	//make sure the new parent isn't empty
	if (const auto& newParent = m_Parent)
		newParent->AddChild(this);

}

GameObject* GameObject::GetChildAt(unsigned int index) const
{
	if (index > m_Childeren.size()) 
		return m_Childeren[index];
	return nullptr;
}

bool GameObject::RemoveChild(const unsigned int index)
{
	if (index > m_Childeren.size())
	{
		//already happens in SetParent()
		//m_Childeren[index]->SetParent(std::weak_ptr<GameObject>()); //remove parent
		m_Childeren.erase(m_Childeren.begin() + index);
		return true;
	}
	return false;
}

bool GameObject::RemoveChild(GameObject* go)
{
	const auto i = std::ranges::find(m_Childeren, go);
	if (i != m_Childeren.end())
	{
		m_Childeren.erase(i);
		return true;
	}

	return false;
}

void GameObject::AddChild(GameObject* go)
{
	m_Childeren.emplace_back(go);
	//already happens in SetParent() (same for removing the parent)
	//go->SetParent(shared_from_this());
}

const Transform* GameObject::GetTransform()
{
	if (m_PositionDirty)
		UpdateWorldPosition();
	return m_pTransform;
}

void GameObject::SetLocalPosition(float x, float y)
{
	m_pTransform->SetLocalPosition(x, y, 0);
	SetPositionDirty();
}

void GameObject::SetLocalPosition(const glm::vec2& pos)
{
	m_pTransform->SetLocalPosition(pos.x, pos.y, 0);
	SetPositionDirty();
}

void biggin::GameObject::TranslateLocalPosition(const glm::vec2& pos)
{
	m_pTransform->TranslateLocalPosition({ pos.x, pos.y, 0 });
	SetPositionDirty();
}

const glm::vec2 GameObject::GetLocalPosition()
{
	return m_pTransform->GetLocalPosition();
}

const glm::vec2 GameObject::GetWorldPosition()
{
	if (m_PositionDirty)
		UpdateWorldPosition();

	return m_pTransform->GetWorldPosition();
}

void biggin::GameObject::SetPositionDirty()
{
	//if dirty flag hasn't already been set; set the dirty flag gon all children aswell
	if (!m_PositionDirty){
		for (const auto child : m_Childeren)
			child->SetPositionDirty();
	}

	m_PositionDirty = true;
}

void biggin::GameObject::SetSceneRef(Scene* scene)
{
	if (m_SceneRef == nullptr)
		m_SceneRef = scene;
	else
	{
		m_SceneRef->Remove(this);
		m_SceneRef = scene;
	}
}

void biggin::GameObject::RemoveObserver(Observer* observer)
{
	if (auto subjectComponent = GetComponent<Subject>())
		subjectComponent->RemoveObserver(observer);
}

void biggin::GameObject::RemoveObservers(const std::vector<Observer*>& observers)
{
	if (auto subjectComponent = GetComponent<Subject>())
	{
		for (const auto observer : observers)
		{
			subjectComponent->RemoveObserver(observer);
		}
	}
}

void biggin::GameObject::AddObserver(Observer* observer)
{
	if (auto subjectComponent = GetComponent<Subject>())
		subjectComponent->AddObserver(observer);
}

void biggin::GameObject::AddObservers(const std::vector<Observer*>& observers)
{
	if (auto subjectComponent = GetComponent<Subject>())
	{
		for (const auto observer : observers)
		{
			subjectComponent->AddObserver(observer);
		}
	}
}

void GameObject::UpdateWorldPosition()
{
	if (m_PositionDirty)
	{
		if (!m_Parent)
			m_pTransform->SetWorldPosition(m_pTransform->GetLocalPosition());
		else
			m_pTransform->SetWorldPosition( m_Parent->GetWorldPosition()+m_pTransform->GetLocalPosition());
	}
	m_PositionDirty = false;
}

void GameObject::AddComponent(Component* component)
{
	m_Components.emplace_back(component);
}

void GameObject::AddComponentPending(Component* component)
{
	m_pPendingAdd.emplace_back(component);
}

