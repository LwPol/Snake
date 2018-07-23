#include "stdafx.h"
//#include "snakeWndProc.h"
#include "startingMenu.h"

int WINAPI wWinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPWSTR lpCmdLine, int nShowCmd) {
	Application::setIcon(LoadIcon(nullptr, IDI_APPLICATION));
	Application::setSmallIcon(LoadIcon(nullptr, IDI_APPLICATION));
	Application::setCursor(LoadCursor(nullptr, IDC_ARROW));
	Application::registerWindowClass();

	//Application::setNewWindowProc<SnakeWndProc>();
	Application::setNewWindowProc<StartingMenu>();

	if (!Application::createMainWindow(CW_USEDEFAULT, 0, L"Snake")) {
		MessageBox(nullptr, L"Utworzenie okna nie powiod³o siê", nullptr, MB_ICONINFORMATION);
		return 0;
	}

	try {
		return Application::run();
	}
	catch (std::exception &ex) {
		MessageBoxA(Application::hWnd(), ex.what(), "Error", MB_ICONSTOP);
		Application::setNewWindowProc<void>();
		return -1;
	}
}
