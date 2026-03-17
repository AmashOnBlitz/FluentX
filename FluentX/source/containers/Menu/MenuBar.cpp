#include "core/pch.h"
#include "containers/Menu/MenuBar.h"

NAMESPACE_FLUENTX::MenuBar::MenuBar()
{

}

NAMESPACE_FLUENTX::MenuBar::~MenuBar()
{
}

void NAMESPACE_FLUENTX::MenuBar::AddMenu(Menu* menu)
{
    menu->_SetParent(this);
	this->mVectMenus.push_back(menu);
    this->FlagRebuild();
}

void NAMESPACE_FLUENTX::MenuBar::InsertMenuAt(Menu* menu, int pos)
{
    if (pos < 0) pos = 0;
    if (pos > static_cast<int>(mVectMenus.size())) 
        pos = static_cast<int>(mVectMenus.size());
    menu->_SetParent(this);
    mVectMenus.insert(mVectMenus.begin() + pos, menu);
    this->FlagRebuild();
}


void NAMESPACE_FLUENTX::MenuBar::RemoveMenu(const std::string& name)
{
    for (auto it = mVectMenus.begin(); it != mVectMenus.end(); ++it)
    {
        if (it.operator*()->GetLabel() == name)
        {
            mVectMenus.erase(it);
            break;
        }
    }
    this->FlagRebuild();
}

void NAMESPACE_FLUENTX::MenuBar::FlagRebuild(bool rebuild)
{
    //std::cout << "Flag received in Menu Bar, current flag : " << rebuild << "\n";
    rebuildMenu = rebuild;
}

bool NAMESPACE_FLUENTX::MenuBar::GetRebuild()
{
    return rebuildMenu;
}

std::vector<NAMESPACE_FLUENTX::Menu*>& NAMESPACE_FLUENTX::MenuBar::GetVectMenus()
{
    return mVectMenus;
}

