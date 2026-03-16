#include "containers/window/MainWindow.h"
#include "containers/window/MainWindowStyle.h"
#include "core/App.h"
#include <iostream>

namespace fx = FluentX;

int main(int argc, char* argv[]) {
	fx::MainWindow* window = new fx::MainWindow();
	window->Init("First Window", 600, 800, 50, 50, fx::MainWindowStylePresets::ToolPanel);
	window->onClose([](std::string){
		fx::App::Instance().Shutdown();
	});
	window->showWindow();
	fx::App::Instance().SetMainWindow(window);
	return fx::App::Instance().Run();
}