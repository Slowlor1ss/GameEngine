#pragma once
#include "Singleton.h"

namespace biggin
{
	class MenuState;

	class MenuRenderer final : public Singleton<MenuRenderer>
	{
	public:
		void SetMenu(MenuState* menu);
		void RenderMenu() const;

	private:
		friend class Singleton<MenuRenderer>;
		MenuRenderer() = default;
		~MenuRenderer() override;

		MenuState* m_pState{nullptr};
	};
}
