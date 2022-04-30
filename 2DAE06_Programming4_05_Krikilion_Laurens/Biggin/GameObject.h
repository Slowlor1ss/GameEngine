#pragma once

namespace biggin
{
	class Component;
	class Transform;

	class GameObject final
	{
	public:
		GameObject();
		GameObject(GameObject* parent) { SetParent(parent); };
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
		void RenderUi();


		void										AddComponent(Component* component);

		//partly based on https://stackoverflow.com/questions/44105058/implementing-component-system-from-unity-in-c
		template<typename ComponentType>
		ComponentType*								GetComponent();
		template<typename ComponentType>
		std::vector<ComponentType*>					GetComponents();


		template<typename ComponentType>
		bool										RemoveComponent();
		template<typename ComponentType>
		int											RemoveComponents();

		//TODO: for all children set transform relative to the transform of the parent
		void										SetParent(GameObject* parent, bool keepWorldPos = true); //TODO: all children should have this as parent one the new parent is set also check if parent is not itself
		GameObject*									GetParent() const { return m_Parent; };

		size_t										GetChildCount() const { return m_Childeren.size(); };
		GameObject*									GetChildAt(unsigned int index) const;
		std::vector<GameObject*>					GetAllChilderen() const { return m_Childeren; }; //TODO: keep it private


		//void										SetTransform(Transform* transform) { m_Transform = transform; };
		const Transform*							GetTransform();

		void										SetLocalPosition(float x, float y);
		void										SetLocalPosition(const glm::vec2& pos);
		void										TranslateLocalPosition(const glm::vec2& pos);
		const glm::vec2								GetLocalPosition();

		const glm::vec2								GetWorldPosition();

		void										SetPositionDirty() { m_PositionDirty = true; };


	private:
		//TODO: maybe make them private
		//TODO: remove yourself as parent, remove form childeren list update transform etc
		bool										RemoveChild(unsigned int index);
		bool										RemoveChild(GameObject* go);
		//TODO: set parent, update transform rot and scale, if already parented remove that parent and change it to this parent
		void										AddChild(GameObject* go);//TODO: first check if the child already has a parent and if it does remove that parent from the child maybe also check if child is already in there

		void										UpdateWorldPosition();

		//Allows "caching" of the transform component to we don't have to find it in the components list every time
		Transform* m_Transform{ nullptr };
		GameObject* m_Parent{ nullptr };
		std::vector<Component*> m_Components; //TODO: maybe make these raw ptrs
		std::vector<GameObject*> m_Childeren;

		bool m_PositionDirty{false};
	};

	/**
	 * \brief Gets first added component of type <ComponentType>
	 * \tparam ComponentType The type of which it will get the component
	 * \return shared pointer to the found component of type <ComponentType>
	 */
	template<typename ComponentType>
	ComponentType* GameObject::GetComponent()
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

		bool success = index != m_Components.end();

		if (success)
			m_Components.erase(index); //TODO: if we use raw pointers later we should also delete the object here

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
		bool success = false;

		do
		{
			auto& index = std::find_if(m_Components.begin(),
				m_Components.end(),
				[](auto& component)
				{
					return dynamic_cast<ComponentType*>(component);
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
