#include "core/pch.h"
#include "containers/window/window.h"
#include "core/macroFuncs.h"
#include <containers/window/MainWindow.h>
#include <dwmapi.h>
#include "tinyxml2/tinyxml2.h"
#include <filesystem>

NAMESPACE_FLUENTX::Window::Window()
{
	mWndContext = new WindowContext{};
}


NAMESPACE_FLUENTX::Window::~Window()
{
	if (mWndContext) delete mWndContext;
	mWndContext = nullptr;
}

WindowContext& NAMESPACE_FLUENTX::Window::getWndContext()
{
	if (!mWndContext) {
		FLUENTX_THROW_ERROR("WindowContext not set!");
	}
	return (*this->mWndContext);
}

bool NAMESPACE_FLUENTX::Window::setWndContext(WindowContext& wndCont)
{
	this->mWndContext = &wndCont;
	return true;
}

bool NAMESPACE_FLUENTX::Window::ShowWindow()
{
	if (mWndContext) {
		if (mWndContext->hWnd != INVALID_HANDLE_VALUE && mWndContext->hWnd != NULL) {
			::ShowWindow(mWndContext->hWnd, ConvToWin32WndStyle_Startup(mStyle.startup));
			::UpdateWindow(mWndContext->hWnd);
			return true;
		}
	}
	std::string err = "HWND not found, maybe incomplete window creation!";
	this->setLastErr(err);
	FLUENTX_THROW_ERROR(err);
	return false;
}

bool NAMESPACE_FLUENTX::Window::HideWindow()
{
	if (mWndContext) {
		if (mWndContext->hWnd != INVALID_HANDLE_VALUE && mWndContext->hWnd != NULL) {
			::ShowWindow(mWndContext->hWnd, SW_HIDE);
			return true;
		}
	}
	std::string err = "HWND not found, maybe incomplete window creation!";
	this->setLastErr(err);
	FLUENTX_THROW_ERROR(err);
	return false;
}

void NAMESPACE_FLUENTX::Window::onClose(OnWindowClose func)
{
	mOnClose = func;
}

void NAMESPACE_FLUENTX::Window::BeforeClose(BeforeWindowClose func)
{
	mBeforeClose = func;
}

std::string NAMESPACE_FLUENTX::Window::FetchLastErr()
{
	std::string err = this->errStr;
	this->errStr = "";
	return err;
}

void NAMESPACE_FLUENTX::Window::SetPos(int x, int y)
{
	SetWindowPos(
		this->getWndContext().hWnd,
		NULL,
		x,
		y,
		0,
		0,
		SWP_NOSIZE | SWP_NOZORDER
	);
}

void NAMESPACE_FLUENTX::Window::SetDimensions(int width, int height)
{
	SetWindowPos(
		this->getWndContext().hWnd,
		NULL,
		0,
		0,
		width,
		height,
		SWP_NOMOVE | SWP_NOZORDER
	);
}

void NAMESPACE_FLUENTX::Window::SetBounds(int x, int y, int width, int height)
{
	SetWindowPos(
		this->getWndContext().hWnd,
		NULL,
		x,
		y,
		width,
		height,
		SWP_NOZORDER
	);
}

int NAMESPACE_FLUENTX::Window::GetPosX()
{
	GetWindowRect(
		this->getWndContext().hWnd,
		&mRect
	);
	return mRect.left;
}

int NAMESPACE_FLUENTX::Window::GetPosY()
{
	GetWindowRect(
		this->getWndContext().hWnd,
		&mRect
	);
	return mRect.top;
}

int NAMESPACE_FLUENTX::Window::GetWidth()
{
	GetWindowRect(
		this->getWndContext().hWnd,
		&mRect
	);
	return (mRect.right - mRect.left);
}

int NAMESPACE_FLUENTX::Window::GetHeight()
{
	GetWindowRect(
		this->getWndContext().hWnd,
		&mRect
	);
	return (mRect.bottom - mRect.top);
}

bool NAMESPACE_FLUENTX::Window::saveWindowData()
{
	namespace fs = std::filesystem;

	if (!this->mWndContext->hWnd)
	{
		std::string err = "HWND not found, maybe incomplete window creation!";
		this->setLastErr(err);
		FLUENTX_THROW_ERROR(err);
		return false;
	}

	int x = this->GetPosX();
	int y = this->GetPosY();
	int w = this->GetWidth();
	int h = this->GetHeight();

	std::string basePath = GetLocalAppDataPath();
	std::string folderPath = basePath + "\\WindowMetaData";
	fs::create_directories(folderPath);

	std::string filePath = folderPath + "\\WindowStates.xml";
	std::string windowName = GetWindowTitle(this->mWndContext->hWnd);

	tinyxml2::XMLDocument doc;

	if (fs::exists(filePath))
	{
		if (doc.LoadFile(filePath.c_str()) != tinyxml2::XML_SUCCESS)
		{
			doc.Clear();
		}
	}

	tinyxml2::XMLElement* root = doc.FirstChildElement("Windows");
	if (!root)
	{
		root = doc.NewElement("Windows");
		doc.InsertFirstChild(root);
	}

	tinyxml2::XMLElement* windowElem = root->FirstChildElement("Window");
	while (windowElem)
	{
		const char* nameAttr = windowElem->Attribute("title");
		if (nameAttr && windowName == nameAttr)
			break;
		windowElem = windowElem->NextSiblingElement("Window");
	}

	if (!windowElem)
		windowElem = doc.NewElement("Window");

	windowElem->SetAttribute("title", windowName.c_str());

	tinyxml2::XMLElement* posElem = windowElem->FirstChildElement("Position");
	if (!posElem)
		posElem = doc.NewElement("Position");

	posElem->SetAttribute("x", x);
	posElem->SetAttribute("y", y);

	tinyxml2::XMLElement* sizeElem = windowElem->FirstChildElement("Size");
	if (!sizeElem)
		sizeElem = doc.NewElement("Size");

	sizeElem->SetAttribute("width", w);
	sizeElem->SetAttribute("height", h);

	if (!windowElem->FirstChildElement("Position"))
		windowElem->InsertEndChild(posElem);
	if (!windowElem->FirstChildElement("Size"))
		windowElem->InsertEndChild(sizeElem);

	if (!windowElem->Parent())
		root->InsertEndChild(windowElem);

	doc.SaveFile(filePath.c_str());
	return true;
}

void NAMESPACE_FLUENTX::Window::loadWindowData()
{
	namespace fs = std::filesystem;

	if (!this->mWndContext->hWnd)
	{
		std::string err = "HWND not found, cannot load window data!";
		this->setLastErr(err);
		FLUENTX_THROW_ERROR(err);
		return;
	}

	std::string basePath = GetLocalAppDataPath();
	std::string folderPath = basePath + "\\WindowMetaData";
	std::string filePath = folderPath + "\\WindowStates.xml";
	if (!fs::exists(filePath))
		return;

	tinyxml2::XMLDocument doc;
	if (doc.LoadFile(filePath.c_str()) != tinyxml2::XML_SUCCESS)
		return;

	tinyxml2::XMLElement* root = doc.FirstChildElement("Windows");
	if (!root) return;;

	std::string windowName = GetWindowTitle(this->mWndContext->hWnd);
	tinyxml2::XMLElement* windowElem = root->FirstChildElement("Window");

	while (windowElem)
	{
		const char* nameAttr = windowElem->Attribute("title");
		if (nameAttr && windowName == nameAttr)
			break;
		windowElem = windowElem->NextSiblingElement("Window");
	}

	if (!windowElem) return;

	tinyxml2::XMLElement* posElem = windowElem->FirstChildElement("Position");
	tinyxml2::XMLElement* sizeElem = windowElem->FirstChildElement("Size");

	if (!posElem || !sizeElem)
		return;

	int x, y, w, h;
	bool valid = true;

	if (posElem->QueryIntAttribute("x", &x) != tinyxml2::XML_SUCCESS) valid = false;
	if (posElem->QueryIntAttribute("y", &y) != tinyxml2::XML_SUCCESS) valid = false;
	if (sizeElem->QueryIntAttribute("width", &w) != tinyxml2::XML_SUCCESS) valid = false;
	if (sizeElem->QueryIntAttribute("height", &h) != tinyxml2::XML_SUCCESS) valid = false;

	if (w <= 0 || h <= 0) valid = false;

	if (valid)
		this->SetBounds(x, y, w, h);
}



void NAMESPACE_FLUENTX::Window::setLastErr(std::string _e)
{
	this->errStr = _e;
}

LRESULT NAMESPACE_FLUENTX::fnMainWinProcNavigator(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	MainWindow* MainWnd = nullptr;
	if (msg == WM_NCCREATE) {
		CREATESTRUCT* cs = reinterpret_cast<CREATESTRUCT*>(lParam);
		MainWnd = reinterpret_cast<MainWindow*>(cs->lpCreateParams);
		::SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)MainWnd);
	}
	else {
		MainWnd = reinterpret_cast<MainWindow*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
	}
	if (MainWnd) {
		return MainWnd->fnWndProc(hwnd, msg, wParam, lParam);
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

DWORD NAMESPACE_FLUENTX::Window::ConvToWin32WndStyle_Creation(MainWindowCreationFlags style)
{
	DWORD flags = 0;

	if ((style & MainWindowCreationFlags::TitleBar) != MainWindowCreationFlags::None)
		flags |= WS_CAPTION;

	if ((style & MainWindowCreationFlags::Border) != MainWindowCreationFlags::None)
		flags |= WS_BORDER;

	if ((style & MainWindowCreationFlags::Resizable) != MainWindowCreationFlags::None)
		flags |= WS_THICKFRAME;

	if ((style & MainWindowCreationFlags::MinimizeButton) != MainWindowCreationFlags::None)
		flags |= WS_MINIMIZEBOX;

	if ((style & MainWindowCreationFlags::MaximizeButton) != MainWindowCreationFlags::None)
		flags |= WS_MAXIMIZEBOX;

	if ((style & MainWindowCreationFlags::SystemMenu) != MainWindowCreationFlags::None)
		flags |= WS_SYSMENU;

	if ((style & MainWindowCreationFlags::Popup) != MainWindowCreationFlags::None)
		flags |= WS_POPUP;

	if ((style & MainWindowCreationFlags::Dialog) != MainWindowCreationFlags::None)
		flags |= WS_DLGFRAME;

	return flags;
}

DWORD NAMESPACE_FLUENTX::Window::ConvToWin32WndExStyle_Creation(MainWindowCreationFlags style)
{
	DWORD flags = 0;

	if ((style & MainWindowCreationFlags::TopMost) != MainWindowCreationFlags::None)
		flags |= WS_EX_TOPMOST;

	if ((style & MainWindowCreationFlags::ToolWindow) != MainWindowCreationFlags::None)
		flags |= WS_EX_TOOLWINDOW;

	if ((style & MainWindowCreationFlags::AppWindow) != MainWindowCreationFlags::None)
		flags |= WS_EX_APPWINDOW;

	if ((style & MainWindowCreationFlags::AcceptFiles) != MainWindowCreationFlags::None)
		flags |= WS_EX_ACCEPTFILES;

	return flags;
}


int NAMESPACE_FLUENTX::Window::ConvToWin32WndStyle_Startup(MainWindowStartupState state)
{
	switch (state)
	{
	case MainWindowStartupState::Normal:         return SW_SHOWNORMAL;
	case MainWindowStartupState::Minimized:      return SW_SHOWMINIMIZED;
	case MainWindowStartupState::Maximized:      return SW_SHOWMAXIMIZED;
	case MainWindowStartupState::Hidden:         return SW_HIDE;
	case MainWindowStartupState::Restore:        return SW_RESTORE;
	case MainWindowStartupState::Show:           return SW_SHOW;
	case MainWindowStartupState::ShowNoActivate: return SW_SHOWNOACTIVATE;
	case MainWindowStartupState::ShowDefault:    return SW_SHOWDEFAULT;
	default:                                     return SW_SHOWNORMAL;
	}
}

void NAMESPACE_FLUENTX::Window::ApplyUIFlags(HWND hwnd, MainWindowUIFlags flags)
{
	if ((flags & MainWindowUIFlags::Layered) != MainWindowUIFlags::None)
	{
		LONG_PTR ex = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
		SetWindowLongPtr(hwnd, GWL_EXSTYLE, ex | WS_EX_LAYERED);
	}

	if ((flags & MainWindowUIFlags::DropShadow) != MainWindowUIFlags::None)
	{
		LONG_PTR ex = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
		SetWindowLongPtr(hwnd, GWL_EXSTYLE, ex | WS_EX_DLGMODALFRAME);
	}

	if ((flags & MainWindowUIFlags::RoundedCorners) != MainWindowUIFlags::None)
	{
		DWM_WINDOW_CORNER_PREFERENCE pref = DWMWCP_ROUND;
		DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &pref, sizeof(pref));
	}
}

void NAMESPACE_FLUENTX::Window::ApplyBehaviorFlags(HWND hwnd, MainWindowBehaviorFlags flags)
{
	if ((flags & MainWindowBehaviorFlags::ClickThrough) != MainWindowBehaviorFlags::None)
	{
		LONG ex = GetWindowLong(hwnd, GWL_EXSTYLE);
		SetWindowLong(hwnd, GWL_EXSTYLE, ex | WS_EX_TRANSPARENT);
	}

	if ((flags & MainWindowBehaviorFlags::Transparent) != MainWindowBehaviorFlags::None)
	{
		SetLayeredWindowAttributes(hwnd, 0, 200, LWA_ALPHA);
	}

	if ((flags & MainWindowBehaviorFlags::NoActivate) != MainWindowBehaviorFlags::None)
	{
		LONG ex = GetWindowLong(hwnd, GWL_EXSTYLE);
		SetWindowLong(hwnd, GWL_EXSTYLE, ex | WS_EX_NOACTIVATE);
	}
}

std::string NAMESPACE_FLUENTX::Window::GetWindowTitle(HWND hwnd)
{
	if (!hwnd) return "window";
	int len = ::GetWindowTextLengthA(hwnd);
	if (len == 0) return "window";
	std::string title(len, '\0');
	::GetWindowTextA(hwnd, title.data(), len + 1);
	return title;
}