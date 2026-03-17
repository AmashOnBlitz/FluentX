#include "core/pch.h"
#include "containers/Menu/Menu.h"
#include "containers/Menu/MenuBar.h"

NAMESPACE_FLUENTX::Menu::Menu(const std::string& label, bool enabled)
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

void NAMESPACE_FLUENTX::Menu::AddMenuItem(MenuItem* mItem)
{
    mItem->_SetParent(this);
    this->mVectMenuItems.push_back(mItem);
    this->FlagParentRebuild();
}

void NAMESPACE_FLUENTX::Menu::AddMenuItemWithSub(MenuItem* item, Menu* subMenu)
{
    item->_SetParent(this);
    item->_SetIsSubMenu(true);
    item->_SetSubMenu(subMenu);
    if (item->_GetIsSubMenu()) {
        item->_GetSubMenu()->_SetParent(this->parentBar);
    }
    this->mVectMenuItems.push_back(item);
    this->FlagParentRebuild();
}

void NAMESPACE_FLUENTX::Menu::AddSeprator()
{
    MenuItem* itm = new MenuItem(FLUENTX_STR_MENU_SEP,[](MenuItem*) {});
    itm->_SetParent(this);
    this->mVectMenuItems.push_back(itm);
    this->FlagParentRebuild();
}

void NAMESPACE_FLUENTX::Menu::InsertMenuItemAt(MenuItem* mItem, int pos)
{
    if (pos < 0) pos = 0;
    if (pos > static_cast<int>(mVectMenuItems.size()))
        pos = static_cast<int>(mVectMenuItems.size());
    mItem->_SetParent(this);
    mVectMenuItems.insert(mVectMenuItems.begin() + pos, mItem);
    this->FlagParentRebuild();
}

void NAMESPACE_FLUENTX::Menu::RemoveMenuItem(const std::string& name)
{
    for (auto it = mVectMenuItems.begin(); it != mVectMenuItems.end(); ++it)
    {
        if (it.operator*()->GetLabel() == name)
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

bool NAMESPACE_FLUENTX::Menu::IsEnabled()
{
    return mEnabled;
}

std::vector<NAMESPACE_FLUENTX::MenuItem*>& NAMESPACE_FLUENTX::Menu::GetVectMenuItems()
{
    return mVectMenuItems;
}

// USED INTERNALLY ---------------------------------
void NAMESPACE_FLUENTX::Menu::_SetParent(MenuBar* bar)
{
    parentBar = bar;
}

// USED INTERNALLY -------------------------------
void NAMESPACE_FLUENTX::Menu::_ReceiveRebuildFlag()
{
    //std::cout << "Rebuild Flag Received in Menu\n";
    this->FlagParentRebuild();
}

void NAMESPACE_FLUENTX::Menu::FlagParentRebuild()
{
    //std::cout << "Flagging Menu Bar to Rebuild\n";
    if (parentBar) 
    {
        parentBar->FlagRebuild();
    }
    else {
        //std::cout << "No Parent Bar!\n";
    }
}
