#pragma once
#include "containers/window/MainWindow.h"
#include <vector>

namespace NAMESPACE_FLUENTX {

    class App {
    public:
        static App& Instance();

        App(const App&) = delete;
        App(App&&) = delete;
        App& operator=(const App&) = delete;
        App& operator=(App&&) = delete;

        void SetMainWindow(MainWindow* wnd);
        void RegisterWindow(MainWindow* wnd);
        void UnregisterWindow(MainWindow* wnd);

        int Run();
        void Shutdown();

    private:
        App() = default;
        ~App() = default;
    private:
        MainWindow* mMainWindow = nullptr;
        std::vector<MainWindow*> mRegisteredWindows;
        bool isEHStarted = false;
    };

} // namespace
