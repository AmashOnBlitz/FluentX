#pragma once
#include "core/config.h"
#include <vector>
#include "Menu.h"

namespace NAMESPACE_FLUENTX {
	class MenuBar {
	public:
		MenuBar();
		~MenuBar();
		void AddMenu(Menu& menu);
		void InsertMenuAt(const Menu& menu, int pos);
		void RemoveMenu(const std::string& name);

		void FlagRebuild();
		bool GetRebuild();

	private:
		std::vector<Menu> mVectMenus;
		bool rebuildMenu = false;
	};
}