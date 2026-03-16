#include "core/pch.h"
#include "containers/Menu/MenuItem.h"
#include "containers/Menu/Menu.h"
#include "containers/Menu/MenuBar.h"

NAMESPACE_FLUENTX::MenuItem::MenuItem(const std::string& label, OnMenuItemClick onClick, bool enabled, bool checked)
{
	this->SetLabel(label);
	this->OnClick(onClick);
	this->SetEnabled(enabled);
	this->SetChecked(checked);
}

NAMESPACE_FLUENTX::MenuItem::~MenuItem()
{
	if (mParentMenu) mParentMenu = nullptr;
}

void NAMESPACE_FLUENTX::MenuItem::SetLabel(std::string label)
{
	this->mLabel = label;
	this->FlagParentRebuild();
}

std::string NAMESPACE_FLUENTX::MenuItem::GetLabel()
{
	return this->mLabel;
}

void NAMESPACE_FLUENTX::MenuItem::SetEnabled(bool enabled)
{
	this->mEnabled = enabled;
	this->FlagParentRebuild();
}

bool NAMESPACE_FLUENTX::MenuItem::IsEnabled()
{
	return this->mEnabled;
}

void NAMESPACE_FLUENTX::MenuItem::SetChecked(bool checked)
{
	this->mChecked = checked;
	this->FlagParentRebuild();
}

bool NAMESPACE_FLUENTX::MenuItem::IsChecked()
{
	return this->mChecked;
}

void NAMESPACE_FLUENTX::MenuItem::OnClick(OnMenuItemClick onClick)
{
	this->mOnClick = onClick;
	this->FlagParentRebuild();
}

NAMESPACE_FLUENTX::OnMenuItemClick NAMESPACE_FLUENTX::MenuItem::GetOnClick()
{
	return this->mOnClick;
}

// USED INTERNALLY -----------------------------------
void NAMESPACE_FLUENTX::MenuItem::SetParent(Menu* menu)
{
	this->mParentMenu = menu;
}

void NAMESPACE_FLUENTX::MenuItem::FlagParentRebuild()
{
	if (mParentMenu) {
		mParentMenu->_ReceiveRebuildFlag();
	}
}
