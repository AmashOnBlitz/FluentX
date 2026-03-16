#include "containers/window/MainWindow.h"
#include "containers/window/MainWindowStyle.h"
#include "core/App.h"
#include <iostream>

namespace fx = FluentX;

int main(int argc, char* argv[]) {

	fx::MainWindow* window = new fx::MainWindow();
	window->Init("First Window", 600, 800, 50, 50, fx::MainWindowStylePresets::DefaultWindow);
	window->onClose([](std::string){
		fx::App::Instance().Shutdown();
	});
	window->showWindow();
	fx::App::Instance().SetMainWindow(window);

	fx::MainWindow* childWindow = new fx::MainWindow();
	childWindow->Init("Hello From Child", 300, 800, 50, 50, fx::MainWindowStylePresets::DefaultWindow);
	childWindow->onClose([&](std::string) {
		std::cout << "Child Closed!\n";
		fx::App::Instance().UnregisterWindow(childWindow);
	});
	childWindow->setParent(*(window));
	childWindow->showWindow();
	fx::App::Instance().RegisterWindow(childWindow);

	return fx::App::Instance().Run();
}