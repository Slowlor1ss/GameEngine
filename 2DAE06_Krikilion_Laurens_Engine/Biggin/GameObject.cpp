#include "BigginPCH.h"
#include "GameObject.h"
#include "Logger.h"
#include "Texture2D.h"
#include "Utils.hpp"
#include "Transform.h"

dae::GameObject::GameObject()
{
	//TODO: make components raw
	//I am very aware this is a bad thing to do and there a real danger for a dangling ptr
	//but by making my transform raw while my components are smart
	//im basically forcing myself to eventually clean it up
	m_Transform = new Transform(this);
	m_Components.push_back(std::shared_ptr<Transform>(m_Transform));
}

void dae::GameObject::Update()
{
	for (auto &component : m_Components)
	{
		component->Update();
	}
}

void dae::GameObject::FixedUpdate()
{
	for (auto &component : m_Components)
	{
		component->FixedUpdate();
	}
}

void dae::GameObject::Render() const
{
	for (const auto &component : m_Components)
	{
		component->Render();
	}
}

void dae::GameObject::RenderUi()
{
	for (const auto& component : m_Components)
	{
		component->RenderUi();
	}
}

void dae::GameObject::SetParent(std::weak_ptr<GameObject> parent)
{
	//Check if given parent isn't already the parent
	if (utils::equals(m_Parent, parent))
	{
#ifdef _DEBUG
		Logger::GetInstance().LogWarning("SetParent called with current parent as parameter.");
#endif
		return;
	}

	//Check if there already was a parent and if so remove itself as a child
	if (const auto oldParent = m_Parent.lock())
		oldParent->RemoveChild(shared_from_this());

	m_Parent = parent;
	//make sure the new parent isn't empty
	if (const auto newParent = m_Parent.lock())
		newParent->AddChild(shared_from_this());

	//TODO: update pos rot and scale
}

std::shared_ptr<dae::GameObject> dae::GameObject::GetChildAt(unsigned int index) const
{
	if (index > m_Childeren.size()) 
		return m_Childeren[index];
	return nullptr;
}

bool dae::GameObject::RemoveChild(const unsigned int index)
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

bool dae::GameObject::RemoveChild(const std::shared_ptr<GameObject> go)
{
	const auto i = std::ranges::find(m_Childeren, go);
	if (i != m_Childeren.end())
	{
		m_Childeren.erase(i);
		return true;
	}

	return false;
}

void dae::GameObject::AddChild(const std::shared_ptr<GameObject> go)
{
	m_Childeren.push_back(go);
	//already happens in SetParent() (same for removing the parent)
	//go->SetParent(shared_from_this());
}

void dae::GameObject::SetPosition(float x, float y)
{
	m_Transform->SetPosition(x, y, 0);
}

void dae::GameObject::AddComponent(std::shared_ptr<Component> component)
{
	component->Start();
	m_Components.emplace_back(component);
}

