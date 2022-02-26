#pragma once
#include "Transform.h"

class Component;

namespace dae
{
	class GameObject
	{
	public:
		void Update();
		void FixedUpdate();
		void Render() const;

		void AddComponent(std::shared_ptr<Component> component);

		//partly based on https://stackoverflow.com/questions/44105058/implementing-component-system-from-unity-in-c
		template<typename ComponentType>
		std::shared_ptr<ComponentType> GetComponent();
		template<typename ComponentType>
		std::vector<std::shared_ptr<ComponentType>> GetComponents();

		template<typename ComponentType>
		bool RemoveComponent();
		template<typename ComponentType>
		int RemoveComponents();

		void SetParent(std::weak_ptr<GameObject> parent) { m_Parent = parent; };
		std::weak_ptr<GameObject> GetParent() const { return m_Parent; };

		size_t GetChildCount() const { return m_Childeren.size(); };
		std::shared_ptr<GameObject> GetChildAt(unsigned int index) const;
		std::vector<std::shared_ptr<GameObject>> GetAllChilderen() const { return m_Childeren; };
		bool RemoveChild(unsigned int index);
		void AddChild(const std::shared_ptr<GameObject> go) { m_Childeren.push_back(go); };

		void SetPosition(float x, float y);

		GameObject(std::shared_ptr<GameObject> parent = nullptr) : m_Parent(parent) {};
		GameObject(std::weak_ptr<GameObject> parent) : m_Parent(parent) {};
		virtual ~GameObject() {};
		GameObject(const GameObject& other) = delete;
		GameObject(GameObject&& other) = delete; 
		GameObject& operator=(const GameObject& other) = delete;
		GameObject& operator=(GameObject&& other) = delete;

	private:
		Transform m_Transform{};
		std::weak_ptr<GameObject> m_Parent{};
		std::vector<std::shared_ptr<Component>> m_Components;
		std::vector<std::shared_ptr<GameObject>> m_Childeren;
	};

	template<typename ComponentType>
	std::shared_ptr<ComponentType> GameObject::GetComponent()
	{
		for (auto &component : m_Components) 
		{
			//if (component->IsClassType(ComponentType::Type))
			if (auto c = std::dynamic_pointer_cast<ComponentType>(component))
				return c;
				//return *dynamic_cast<ComponentType*>(component.get());
		}

		return nullptr;
	}

	template<typename ComponentType>
	std::vector<std::shared_ptr<ComponentType>> GameObject::GetComponents()
	{
		std::vector<std::shared_ptr<ComponentType>> componentsOfType;

		for (auto& component : m_Components) 
		{
			if (auto c = std::dynamic_pointer_cast<ComponentType>(component))
				componentsOfType.emplace_back(c);
		}

		return componentsOfType;
	}

	template<typename ComponentType>
	bool GameObject::RemoveComponent()
	{
		if (m_Components.empty())
			return false;

		auto& index = std::find_if(m_Components.begin(),
			m_Components.end(),
			[](auto& component) 
			{
				return std::dynamic_pointer_cast<ComponentType>(component);
			});

		bool success = index != m_Components.end();

		if (success)
			m_Components.erase(index);

		return success;
	}

	template<typename ComponentType>
	int GameObject::RemoveComponents()
	{
		if (m_Components.empty())
			return 0;

		int numRemoved = 0;
		bool success = false;

		do
		{
			auto& index = std::find_if(m_Components.begin(),
				m_Components.end(),
				[](auto& component)
				{
					return std::dynamic_pointer_cast<ComponentType>(component);
				});

			success = index != m_Components.end();

			if (success)
			{
				m_Components.erase(index);
				++numRemoved;
			}
		} while (success);

		return numRemoved;
	}

}
