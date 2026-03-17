#pragma once
#include "containers/window/MainWindow.h"
#include <vector>
#include <thread>
#include <mutex>
#include <chrono>

namespace NAMESPACE_FLUENTX {

    typedef std::function<void()> OnUpdateFunc;

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

        void Init();
        int Run();
        void Shutdown();

        OnUpdateFunc getOnUpdate();
        void OnUpdate(OnUpdateFunc func);

        void SetFPS(int fps);
        int GetFPS();

        void CheckMenuRebuilds();

    private:
        App() = default;
        ~App() = default;
    private:
        bool isInit = false;
        std::thread mUpdateThread;
        std::atomic<bool> mRunning = false;
        std::atomic<int> mFps = 20;
        MainWindow* mMainWindow = nullptr;
        std::vector<MainWindow*> mRegisteredWindows;
        bool isEHStarted = false;
        OnUpdateFunc mOnUpdate;
        std::mutex mUpdateMutex;
    };

}
