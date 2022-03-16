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

	    virtual void Start() {};
	    virtual void Update() {};
	    virtual void FixedUpdate() {};
	    virtual void Render() const {};
	    virtual void RenderUi() {};

	protected:
	    Component(GameObject* go) : m_pGameObject(go) {};
	    GameObject* GetGameObject() const { return m_pGameObject; };

	private:
	    GameObject* m_pGameObject;
	};

}
