#pragma once
#include "core/config.h"
#include <functional>

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
		void SetParent(Menu* menu);

	private: // Funcs
		void FlagParentRebuild();
	private: // Vars 
		bool mChecked = false;
		bool mEnabled = true;
		std::string mLabel = "";
		Menu* mParentMenu = nullptr;
		OnMenuItemClick mOnClick;
	};
}