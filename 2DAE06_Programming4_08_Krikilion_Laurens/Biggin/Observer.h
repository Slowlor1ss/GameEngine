#pragma once

namespace biggin
{
	class Component;

	class Observer
	{
	public:
		virtual ~Observer() = default;
		virtual void OnNotify(Component* entity, const std::string& event) = 0;
	};
}
