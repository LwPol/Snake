#include "stdafx.h"
#include "application.h"

HINSTANCE Application::hInst = nullptr;

HWND Application::hMainWindow = nullptr;

WNDCLASSEX Application::wcex = {
	sizeof(WNDCLASSEX),
	CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS,
	&Application::baseWindowProc,
	0,
	0,
	Application::hInst = GetModuleHandle(nullptr),
	nullptr,
	nullptr,
	(HBRUSH)(COLOR_WINDOW + 1),
	nullptr,
	L"MainWindowClass",
	nullptr
};

DWORD Application::dwWindowStyle = WS_OVERLAPPEDWINDOW;

ATOM Application::registerWindowClass() {
	return RegisterClassEx(&wcex);
}

bool Application::createMainWindow(int width, int height, LPCWSTR title, int nShowCmd) {
	if (hMainWindow)
		return false;

	hMainWindow = CreateWindow(wcex.lpszClassName, title, dwWindowStyle,
		CW_USEDEFAULT, 0, width, height, nullptr, nullptr, hInst, nullptr);
	if (hMainWindow) {
		ShowWindow(hMainWindow, nShowCmd);
		UpdateWindow(hMainWindow);
	}

	return hMainWindow != nullptr;
}

int Application::run() {
	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0) > 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	setNewWindowProc<void>();
	return msg.wParam;
}

LRESULT CALLBACK Application::baseWindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	int ret;
	if (WindowProc::instance)
		ret = WindowProc::instance->wndProc(hWnd, msg, wParam, lParam);
	else
		ret = DefWindowProc(hWnd, msg, wParam, lParam);

	if (msg == WM_DESTROY && hWnd == hMainWindow)
		PostQuitMessage(0);

	return ret;
}

void Application::setIcon(HICON hIcon) {
	wcex.hIcon = hIcon;
}

void Application::setSmallIcon(HICON hIcon) {
	wcex.hIconSm = hIcon;
}

void Application::setCursor(HCURSOR hCursor) {
	wcex.hCursor = hCursor;
}

void Application::setWindowStyle(DWORD style) {
	dwWindowStyle = style;
}

DWORD Application::getWindowStyle() {
	return dwWindowStyle;
}