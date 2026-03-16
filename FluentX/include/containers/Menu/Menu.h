#pragma once
#include "core/config.h"
#include "MenuItem.h"

namespace NAMESPACE_FLUENTX {

	class MenuBar;

	class Menu {
	public:

		Menu(std::string& label, bool enabled);
		~Menu();
		void SetLabel(std::string label);
		std::string GetLabel();

		void AddMenuItem(MenuItem& mItem);
		void InsertMenuItemAt(MenuItem& mItem, int pos);
		void RemoveMenuItem(std::string name);

		void SetEnabled(bool Enabled);
		bool GetEnabled();

		// Used Internally ---------
		void SetParent(MenuBar* bar);
		// USED INTERNALLY -------
		void _ReceiveRebuildFlag();

	private: // Funcs
		void FlagParentRebuild();

	private: // Vars
		std::vector<MenuItem> mVectMenuItems;
		bool mEnabled = true;
		std::string mLabel = "";
		MenuBar* parentBar = nullptr;
	};
}