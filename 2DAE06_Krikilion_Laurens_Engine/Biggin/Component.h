#pragma once

namespace biggin
{
	class GameObject;

	class Component
	{
	public:
	    virtual ~Component() = default;
	    Component(const Component& other) = delete;
	    Component(Component&& other) noexcept = delete;
	    Component& operator=(const Component& other) = delete;
	    Component& operator=(Component&& other) noexcept = delete;

	    virtual void Start() = 0;
	    virtual void Update() = 0;
	    virtual void FixedUpdate() = 0;
	    virtual void Render() const = 0;
	    virtual void RenderUi() = 0;

	protected:
	    Component(GameObject* go) : m_pGameObject(go) {};
	    GameObject* GetGameObject() const { return m_pGameObject; };

	private:
	    GameObject* m_pGameObject;
	};

}
