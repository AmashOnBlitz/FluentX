#include "core/pch.h"
#include "containers/Menu/Menu.h"
#include "containers/Menu/MenuBar.h"

NAMESPACE_FLUENTX::Menu::Menu(std::string& label, bool enabled)
{
    this->SetLabel(label);
    this->SetEnabled(enabled);
}

NAMESPACE_FLUENTX::Menu::~Menu()
{
    if (parentBar) parentBar = nullptr;
}

void NAMESPACE_FLUENTX::Menu::SetLabel(std::string label)
{
    this->mLabel = label;
    this->FlagParentRebuild();
}

std::string NAMESPACE_FLUENTX::Menu::GetLabel()
{
    return this->mLabel;
}

void NAMESPACE_FLUENTX::Menu::AddMenuItem(MenuItem& mItem)
{
    MenuItem mItemCpy = mItem;
    mItemCpy.SetParent(this);
    this->mVectMenuItems.push_back(std::move(mItemCpy));
    this->FlagParentRebuild();
}

void NAMESPACE_FLUENTX::Menu::InsertMenuItemAt(MenuItem& mItem, int pos)
{
    if (pos < 0) pos = 0;
    if (pos > static_cast<int>(mVectMenuItems.size()))
        pos = static_cast<int>(mVectMenuItems.size());
    MenuItem mItemCpy = mItem;
    mItemCpy.SetParent(this);
    mVectMenuItems.insert(mVectMenuItems.begin() + pos, std::move(mItemCpy));
    this->FlagParentRebuild();
}

void NAMESPACE_FLUENTX::Menu::RemoveMenuItem(std::string name)
{
    for (auto it = mVectMenuItems.begin(); it != mVectMenuItems.end(); ++it)
    {
        if (it->GetLabel() == name)
        {
            mVectMenuItems.erase(it);
            break;
        }
    }
    this->FlagParentRebuild();
}

void NAMESPACE_FLUENTX::Menu::SetEnabled(bool Enabled)
{
    this->mEnabled = Enabled;
    this->FlagParentRebuild();
}

bool NAMESPACE_FLUENTX::Menu::GetEnabled()
{
    return mEnabled;
}

// USED INTERNALLY ---------------------------------
void NAMESPACE_FLUENTX::Menu::SetParent(MenuBar* bar)
{
    parentBar = bar;
}

// USED INTERNALLY -------------------------------
void NAMESPACE_FLUENTX::Menu::_ReceiveRebuildFlag()
{
    this->FlagParentRebuild();
}

void NAMESPACE_FLUENTX::Menu::FlagParentRebuild()
{
    if (parentBar) parentBar->FlagRebuild();
}
