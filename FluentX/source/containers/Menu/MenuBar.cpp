#include "core/pch.h"
#include "containers/Menu/MenuBar.h"

NAMESPACE_FLUENTX::MenuBar::MenuBar()
{

}

NAMESPACE_FLUENTX::MenuBar::~MenuBar()
{
}

void NAMESPACE_FLUENTX::MenuBar::AddMenu(Menu& menu)
{
    Menu menuCopy = menu;
    menuCopy.SetParent(this);
	this->mVectMenus.push_back(std::move(menuCopy));
    this->FlagRebuild();
}

void NAMESPACE_FLUENTX::MenuBar::InsertMenuAt(const Menu& menu, int pos)
{
    if (pos < 0) pos = 0;
    if (pos > static_cast<int>(mVectMenus.size())) 
        pos = static_cast<int>(mVectMenus.size());
    Menu menuCopy = menu;
    menuCopy.SetParent(this);
    mVectMenus.insert(mVectMenus.begin() + pos, std::move(menuCopy));
    this->FlagRebuild();
}


void NAMESPACE_FLUENTX::MenuBar::RemoveMenu(const std::string& name)
{
    for (auto it = mVectMenus.begin(); it != mVectMenus.end(); ++it)
    {
        if (it->GetLabel() == name)
        {
            mVectMenus.erase(it);
            break;
        }
    }
    this->FlagRebuild();
}

void NAMESPACE_FLUENTX::MenuBar::FlagRebuild()
{
    rebuildMenu = true;
}

bool NAMESPACE_FLUENTX::MenuBar::GetRebuild()
{
    return rebuildMenu;
}

