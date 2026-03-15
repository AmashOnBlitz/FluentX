#include "containers/window/MainWindow.h"
#include "core/App.h"
#include <iostream>

namespace fx = FluentX;

int main(int argc, char* argv[]) {
	fx::MainWindow* window = new fx::MainWindow();
	window->Init("First Window", 600, 800, 50, 50);
	window->onClose([](std::string){
		fx::MainWindow* anotherWindow = new fx::MainWindow();
		anotherWindow->Init("Another Window", 400, 600, 100, 100);
		anotherWindow->onClose([](std::string) {
			fx::App::Instance().Shutdown();
		});
		fx::App::Instance().SetMainWindow(anotherWindow);
		anotherWindow->showWindow();
	});
	window->showWindow();



	fx::App::Instance().SetMainWindow(window);
	return fx::App::Instance().Run();
}