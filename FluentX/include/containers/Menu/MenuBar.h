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

		void FlagRebuild(bool rebuild = true);
		bool GetRebuild();

		std::vector<Menu>& GetVectMenus();
	private:
		std::vector<Menu> mVectMenus;
		bool rebuildMenu = false;
	};
}