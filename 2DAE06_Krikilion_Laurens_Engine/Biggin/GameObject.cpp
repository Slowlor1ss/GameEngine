#include "BigginPCH.h"
#include "GameObject.h"
#include "Texture2D.h"

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
		component->Render(m_Transform);
	}
}

std::shared_ptr<dae::GameObject> dae::GameObject::GetChildAt(unsigned int index) const
{
	if (index > m_Childeren.size()) 
		return m_Childeren[index];
	return nullptr;
}

bool dae::GameObject::RemoveChild(unsigned int index)
{
	if (index > m_Childeren.size())
	{
		m_Childeren[index]->SetParent(std::weak_ptr<GameObject>()); //remove parent
		m_Childeren.erase(m_Childeren.begin() + index);
		return true;
	}
	return false;
}

void dae::GameObject::AddChild(const std::shared_ptr<GameObject> go)
{
	m_Childeren.push_back(go);
	go->SetParent(shared_from_this());
}

void dae::GameObject::SetPosition(float x, float y)
{
	m_Transform.SetPosition(x, y, 0.0f);
}

void dae::GameObject::AddComponent(std::shared_ptr<Component> component)
{
	m_Components.emplace_back(component);
}

