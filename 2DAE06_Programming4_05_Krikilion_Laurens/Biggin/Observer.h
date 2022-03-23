#pragma once

namespace biggin
{
	class Component;

	class Observer
	{
	public:
		virtual ~Observer() = default;
		virtual void OnNotify(const Component* entity, const std::string& event) = 0;
	};
}
