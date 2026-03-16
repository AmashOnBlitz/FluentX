#include "containers/window/MainWindow.h"
#include "containers/window/MainWindowStyle.h"
#include "containers/Menu/MenuBar.h"
#include "core/App.h"
#include <iostream>

namespace fx = FluentX;

int main(int argc, char* argv[]) {

	fx::MainWindow* window = new fx::MainWindow();
	window->Init("First Window", 800, 600, 50, 50, fx::MainWindowStylePresets::DefaultWindow);
	window->onClose([&window](std::string){
		fx::App::Instance().Shutdown();
	});

	fx::MenuBar MenuBar;
	fx::Menu Menu("Menu Item 1", true);

	fx::MenuItem itm1("Item 1", [](fx::MenuItem* mItm) {
		std::cout << "Menu Item Clicked : " << mItm->GetLabel() << "\n";
		mItm->SetChecked(!mItm->IsChecked());
	});
	fx::MenuItem itm2("Item 2", [](fx::MenuItem* mItm) {
		std::cout << "Menu Item Clicked : " << mItm->GetLabel() << "\n";
	});
	fx::MenuItem itm3("Item 3", [](fx::MenuItem* mItm) {
		std::cout << "Menu Item Clicked : " << mItm->GetLabel() << "\n";
	});
	Menu.AddMenuItem(itm1);
	Menu.AddMenuItem(itm2);
	Menu.AddMenuItem(itm3);
	MenuBar.AddMenu(Menu);
	window->SetMenuBar(MenuBar);

	window->showWindow();
	fx::App::Instance().SetMainWindow(window);

	fx::App::Instance().OnUpdate([&]() {
		//window->SetBounds(
		//	window->GetPosX() + 1,
		//	window->GetPosY() + 1,
		//	window->GetWidth() + 1,
		//	window->GetHeight() + 1
		//);
	});
	return fx::App::Instance().Run();
}