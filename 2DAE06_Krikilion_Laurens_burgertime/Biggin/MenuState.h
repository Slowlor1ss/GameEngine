#pragma once

namespace biggin
{
	class MenuState
	{
	public:
		MenuState() = default;
		virtual ~MenuState() = default;

		virtual void RenderMenu() = 0;
	};
}

