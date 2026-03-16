#include "containers/window/MainWindow.h"
#include "containers/window/MainWindowStyle.h"
#include "core/App.h"
#include <iostream>

namespace fx = FluentX;

int main(int argc, char* argv[]) {

	fx::MainWindow* window = new fx::MainWindow();
	window->Init("First Window", 800, 600, 50, 50, fx::MainWindowStylePresets::DefaultWindow);
	window->onClose([&window](std::string){
		fx::App::Instance().Shutdown();
	});
	window->showWindow();
	fx::App::Instance().SetMainWindow(window);

	fx::App::Instance().OnUpdate([&]() {
		window->SetBounds(
			window->GetPosX() + 0.1,
			window->GetPosY() + 1,
			window->GetWidth() + 1,
			window->GetHeight() + 1
		);
	});
	return fx::App::Instance().Run();
}