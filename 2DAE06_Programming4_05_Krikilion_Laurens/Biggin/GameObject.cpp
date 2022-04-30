#include "BigginPCH.h"
#include "GameObject.h"
#include "Logger.h"
#include "Utils.hpp"
#include "Transform.h"
#include "Component.h"

using namespace biggin;

GameObject::GameObject() : m_Transform(new Transform(this))
{
	m_Components.push_back(m_Transform);
}

biggin::GameObject::~GameObject()
{
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

void GameObject::RenderUi()
{
	for (auto* pComponent : m_Components)
	{
		pComponent->RenderUi();
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

	//TODO: update pos rot and scale
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
	m_Childeren.push_back(go);
	//already happens in SetParent() (same for removing the parent)
	//go->SetParent(shared_from_this());
}

const Transform* GameObject::GetTransform()
{
	if (m_PositionDirty)
		UpdateWorldPosition();
	return m_Transform;
}

void GameObject::SetLocalPosition(float x, float y)
{
	m_Transform->SetLocalPosition(x, y, 0);
	m_PositionDirty = true;
}

void GameObject::SetLocalPosition(const glm::vec2& pos)
{
	m_Transform->SetLocalPosition(pos.x, pos.y, 0);
	m_PositionDirty = true;
}

void biggin::GameObject::TranslateLocalPosition(const glm::vec2& pos)
{
	m_Transform->TranslateLocalPosition({ pos.x, pos.y, 0 });
	m_PositionDirty = true;
}

const glm::vec2 GameObject::GetLocalPosition()
{
	return m_Transform->GetLocalPosition();
}

const glm::vec2 GameObject::GetWorldPosition()
{
	if (m_PositionDirty)
		UpdateWorldPosition();
	return m_Transform->GetWorldPosition();
}

void GameObject::UpdateWorldPosition()
{
	if (m_PositionDirty)
	{
		if (!m_Parent)
			m_Transform->SetWorldPosition(m_Transform->GetLocalPosition());
		else
			m_Transform->TranslateWorldPosition(m_Transform->GetLocalPosition()); //TODO: chnage this to make it more redable
	}
	m_PositionDirty = false;
}

void GameObject::AddComponent(Component* component)
{
	m_Components.emplace_back(component);
}

