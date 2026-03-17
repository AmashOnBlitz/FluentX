#pragma once
#include "core/config.h"
#include <functional>

#define FLUENTX_STR_MENU_SEP "__FLUENTX__INTERNAL__MENU__SEPRATOR__"

namespace NAMESPACE_FLUENTX {

	class Menu;
	class MenuItem;
	typedef std::function<void(MenuItem* mItem)> OnMenuItemClick;
	class MenuItem {
	public:

		MenuItem(const std::string& label, OnMenuItemClick onClick, bool enabled = true, bool checked = false);
		~MenuItem();

		void SetLabel(std::string label);
		std::string GetLabel();

		void SetEnabled(bool enabled);
		bool IsEnabled();

		void SetChecked(bool checked);
		bool IsChecked();

		void OnClick(OnMenuItemClick onClick);
		OnMenuItemClick GetOnClick();

		// USED INTERNALLY -------
		void _SetParent(Menu* menu);
		// USED INTERNALLY -------
		void _SetSubMenu(Menu* menu);
		// USED INTERNALLY -------
		Menu* _GetSubMenu();
		// USED INTERNALLY -------
		void _SetIsSubMenu(bool _isSubMenu);
		// USED INTERNALLY -------
		bool _GetIsSubMenu();
	private: // Funcs
		void FlagParentRebuild();
	private: // Vars 
		bool mChecked = false;
		bool mEnabled = true;
		std::string mLabel = "";
		Menu* mParentMenu = nullptr;
		OnMenuItemClick mOnClick;

		Menu* mSubMenu = nullptr; // OPTIONAL : To support sub menu functionality 
		bool isSubMenu = false; // OPTIONAL : To support sub menu functionality 
	};
}