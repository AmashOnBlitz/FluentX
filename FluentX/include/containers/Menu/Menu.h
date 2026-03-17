#pragma once
#include "core/config.h"
#include "MenuItem.h"

namespace NAMESPACE_FLUENTX {

	class MenuBar;

	class Menu {
	public:

		Menu(const std::string& label, bool enabled);
		~Menu();
		void SetLabel(std::string label);
		std::string GetLabel();

		void AddMenuItem(MenuItem* mItem);
		void AddMenuItemWithSub(MenuItem* item, Menu* subMenu);
		void AddSeprator();
		void InsertMenuItemAt(MenuItem* mItem, int pos);
		void RemoveMenuItem(const std::string& name);

		void SetEnabled(bool Enabled);
		bool IsEnabled();

		std::vector<MenuItem*>& GetVectMenuItems();

		// Used Internally ---------
		void _SetParent(MenuBar* bar);
		// USED INTERNALLY -------
		void _ReceiveRebuildFlag();

	private: // Funcs
		void FlagParentRebuild();

	private: // Vars
		std::vector<MenuItem*> mVectMenuItems;
		bool mEnabled = true;
		std::string mLabel = "";
		MenuBar* parentBar = nullptr;
	};
}