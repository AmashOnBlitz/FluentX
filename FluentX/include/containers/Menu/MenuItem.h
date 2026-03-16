#pragma once
#include "core/config.h"
#include <functional>

namespace NAMESPACE_FLUENTX {

	class Menu;
	typedef std::function<void(std::string)> OnMenuItemClick;
	class MenuItem {
	public:

		MenuItem(std::string& label, OnMenuItemClick onClick, bool enabled = true, bool checked = true);
		~MenuItem();

		void SetLabel(std::string label);
		std::string GetLabel();

		void SetEnabled(bool enabled);
		bool GetEnabled();

		void SetChecked(bool checked);
		bool GetChecked();

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