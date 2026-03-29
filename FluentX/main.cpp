#include "containers/window/MainWindow.h"
#include "containers/window/MainWindowStyle.h"
#include "containers/Menu/MenuBar.h"
#include "core/App.h"
#include <iostream>

namespace fx = FluentX;

int main(int argc, char* argv[]) {
	fx::App::Instance().Init();
	fx::App::Instance().EnableDebugCout(true);

	fx::MainWindowTransitionSet mWndTrSet{};
	mWndTrSet.minimize.enabled = true;
	mWndTrSet.minimize.scale = true;
	mWndTrSet.minimize.slide = true;
	mWndTrSet.minimize.fade = true;

	fx::MainWindow* window = new fx::MainWindow();
	window->Init("First Window", 1600, 800, 50, 50, nullptr, fx::MainWindowStylePresets::DefaultWindow, mWndTrSet);
	window->loadWindowData();
	window->BeforeClose([&window](std::string) {
		window->saveWindowData();
		return true;
	});
	window->onClose([&window](std::string){
		fx::App::Instance().Shutdown();
	});
	window->OnKeyDown([](fx::KeyEvent e) {
		if (e.keyType == fx::Key::Character) {
			std::cout << "Key Pressed : " << e.ch << "\n";
			if (e.state == fx::KeyEventState::NotHandled) {
				std::cout << "Event Ready To be Handled!" << std::endl;
			}
		}
	});
	window->OnKeyUp([](fx::KeyEvent e) {
		if (e.keyType == fx::Key::Character) {
			std::cout << "Key Up\n";
			std::cout << "Key Pressed : " << e.ch << "\n";
			if (e.state == fx::KeyEventState::NotHandled) {
				std::cout << "Event Ready To be Handled!" << std::endl;
			}
		}
	});
	fx::MenuBar* menuBar = new fx::MenuBar();

	fx::Menu* menu1 = new fx::Menu("File", true);
	fx::MenuItem* newItem = new fx::MenuItem("New", [](fx::MenuItem* mItm) {
		std::cout << "Clicked: " << mItm->GetLabel() << "\n";
	});
	fx::MenuItem* openItem = new fx::MenuItem("Open", [](fx::MenuItem* mItm) {
		std::cout << "Clicked: " << mItm->GetLabel() << "\n";
	});
	fx::MenuItem* saveItem = new fx::MenuItem("Save", [](fx::MenuItem* mItm) {
		std::cout << "Clicked: " << mItm->GetLabel() << "\n";
	});

	fx::Menu* exportSubMenu = new fx::Menu("Export", true);
	fx::MenuItem* pdfItem = new fx::MenuItem("PDF", [](fx::MenuItem* mItm) {
		std::cout << "Exporting to PDF\n";
	});
	fx::MenuItem* docItem = new fx::MenuItem("DOCX", [](fx::MenuItem* mItm) {
		std::cout << "Exporting to DOCX\n";
	});
	exportSubMenu->AddMenuItem(pdfItem);
	exportSubMenu->AddMenuItem(docItem);

	fx::MenuItem* exportItem = new fx::MenuItem("Export", nullptr, true, false);

	menu1->AddMenuItem(newItem);
	menu1->AddMenuItem(openItem);
	menu1->AddMenuItem(saveItem);
	menu1->AddSeprator();
	menu1->AddMenuItemWithSub(exportItem,exportSubMenu);

	fx::Menu* menu2 = new fx::Menu("Edit", true);
	fx::MenuItem* undoItem = new fx::MenuItem("Undo", [](fx::MenuItem* mItm) {
		std::cout << "Undo Action\n";
	});
	fx::MenuItem* redoItem = new fx::MenuItem("Redo", [](fx::MenuItem* mItm) {
		std::cout << "Redo Action\n";
	});
	menu2->AddMenuItem(undoItem);
	menu2->AddMenuItem(redoItem);

	menuBar->AddMenu(menu1);
	menuBar->AddMenu(menu2);

	window->SetMenuBar(menuBar);

	window->ShowWindow();
	fx::App::Instance().SetMainWindow(window);

	fx::App::Instance().OnUpdate([&]() {
		//window->SetBounds(
		//	window->GetPosX() + 1,
		//	window->GetPosY() + 1,
		//	window->GetWidth() + 1,
		//);
	});
	return fx::App::Instance().Run();
}