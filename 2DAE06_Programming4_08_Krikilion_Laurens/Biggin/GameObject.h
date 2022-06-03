#pragma once
#include <vector>
#include <string>

namespace biggin
{
	class Observer;
	class Scene;
	class Component;
	class Transform;

	class GameObject final
	{
	public:
		GameObject();
		GameObject(GameObject* parent);
		~GameObject();

		GameObject(const GameObject& other) = delete;
		GameObject(GameObject&& other) = delete; 
		GameObject& operator=(const GameObject& other) = delete;
		GameObject& operator=(GameObject&& other) = delete;

		void Initialize(GameObject* go);
		void Start();
		void Update();
		void FixedUpdate();
		void Render() const;
		void RenderDebug();


		void										AddComponent(Component* component);
		void										AddComponentPending(Component* component);

		//partly based on https://stackoverflow.com/questions/44105058/implementing-component-system-from-unity-in-c
		template<typename ComponentType>
		ComponentType*								GetComponent() const;
		template<typename ComponentType>
		std::vector<ComponentType*>					GetComponents();


		template<typename ComponentType>
		bool										RemoveComponent();
		template<typename ComponentType>
		int											RemoveComponents();
		template<typename ComponentType>
		int											RemoveComponentsPending();

		void										Setname(std::string name) { m_Name = name; };
		std::string									Getname() const { return m_Name; };

		void										SetParent(GameObject* parent, bool keepWorldPos = true);
		GameObject*									GetParent() const { return m_Parent; };

		size_t										GetChildCount() const { return m_Childeren.size(); };
		GameObject*									GetChildAt(unsigned int index) const;
		std::vector<GameObject*>					GetAllChilderen() const { return m_Childeren; };


		//void										SetTransform(Transform* transform) { m_Transform = transform; };
		const Transform*							GetTransform();

		void										SetLocalPosition(float x, float y);
		void										SetLocalPosition(const glm::vec2& pos);
		void										TranslateLocalPosition(const glm::vec2& pos);
		const glm::vec2								GetLocalPosition();

		const glm::vec2								GetWorldPosition();

		void										SetPositionDirty();
		bool										GetPositionDirty() const { return m_PositionDirty; };

		void										SetSceneRef(Scene* scene);
		Scene*										GetSceneRef() const { return m_SceneRef; };

		void										RemoveObserver(Observer* observer);
		void										RemoveObservers(const std::vector<Observer*>& observers);
		void										AddObserver(Observer* observer);
		void										AddObservers(const std::vector<Observer*>& observers);


	private:
		//TODO: maybe make them private
		//TODO: remove yourself as parent, remove form childeren list update transform etc
		bool										RemoveChild(unsigned int index);
		bool										RemoveChild(GameObject* go);
		//TODO: set parent, update transform rot and scale, if already parented remove that parent and change it to this parent
		void										AddChild(GameObject* go);//TODO: first check if the child already has a parent and if it does remove that parent from the child maybe also check if child is already in there

		void										UpdateWorldPosition();

		//Allows "caching" of the transform component to we don't have to find it in the components list every time
		Transform* m_pTransform { nullptr };
		bool m_PositionDirty{false};

		GameObject* m_Parent{ nullptr };
		std::vector<Component*> m_Components{};
		std::vector<GameObject*> m_Childeren{};

		Scene* m_SceneRef{nullptr};
		std::string m_Name;

		std::vector <Component*> m_pPendingAdd{};
		std::vector <Component*> m_pPendingDelete{};
	};

	/**
	 * \brief Gets first added component of type <ComponentType>
	 * \tparam ComponentType The type of which it will get the component
	 * \return shared pointer to the found component of type <ComponentType>
	 */
	template<typename ComponentType>
	ComponentType* GameObject::GetComponent() const
	{
		for (auto &component : m_Components) 
		{
			//if (component->IsClassType(ComponentType::Type))
			if (auto c = dynamic_cast<ComponentType*>(component))
				return c;
				//return *dynamic_cast<ComponentType*>(component.get());
		}

		return nullptr;
	}

	/**
	 * \brief Gets all components of type <ComponentType>
	 * \tparam ComponentType The type of which it will get all components
	 * \return A vector with shared pointers to all found components of type <ComponentType>
	 */
	template<typename ComponentType>
	std::vector<ComponentType*> GameObject::GetComponents()
	{
		std::vector<ComponentType*> componentsOfType;

		for (auto& component : m_Components) 
		{
			if (auto c = dynamic_cast<ComponentType*>(component))
				componentsOfType.emplace_back(c);
		}

		return componentsOfType;
	}

	/**
	 * \brief deletes the first component of <ComponentType>
	 * \tparam ComponentType The type of which this function will delete the first components of
	 * \return Whether or not the operation succeeded
	 */
	template<typename ComponentType>
	bool GameObject::RemoveComponent()
	{
		if (m_Components.empty())
			return false;

		auto& index = std::find_if(m_Components.begin(),
			m_Components.end(),
			[](auto& component) 
			{
				return dynamic_cast<ComponentType*>(component);
			});

		const bool success = index != m_Components.end();

		if (success)
		{
			delete* index;
			m_Components.erase(index);
		}

		return success;
	}

	/**
	 * \brief Deletes all components of type <ComponentType>
	 * \tparam ComponentType The type of which this function will delete all components of
	 * \return Amount of deleted items
	 */
	template<typename ComponentType>
	int GameObject::RemoveComponents()
	{
		if (m_Components.empty())
			return 0;

		int numRemoved = 0;
		//bool success = false;
		//
		//do
		//{
		//	const auto& index = std::find_if(m_Components.begin(),
		//		m_Components.end(),
		//		[](auto& component)
		//		{
		//			return dynamic_cast<ComponentType*>(component);
		//		});
		//
		//	success = index != m_Components.end();
		//
		//	if (success)
		//	{
		//		m_Components.erase(index);
		//		++numRemoved;
		//	}
		//} while (success);

		for (auto i = m_Components.begin(); i != m_Components.end();)
		{
			if (dynamic_cast<ComponentType*>(*i))
			{
				delete *i;
				*i = nullptr;
				i = m_Components.erase(i);
				++numRemoved;
			}
			else
				++i;
		}

		//std::erase_if(m_Components,
		//			[&](auto& component)
		//			{
		//				if( dynamic_cast<ComponentType*>(component))
		//				{
		//					++numRemoved;
		//					return true;
		//				}
		//				return false;
		//			});

		return numRemoved;
	}

	template<typename ComponentType>
	int GameObject::RemoveComponentsPending()
	{
		if (m_Components.empty())
			return 0;

		int numRemoved = 0;

		for (auto i = m_Components.begin(); i != m_Components.end();)
		{
			if (dynamic_cast<ComponentType*>(*i))
			{
				m_pPendingDelete.emplace_back(*i);
				++numRemoved;
			}

			++i;
		}

		return numRemoved;
	}

}
