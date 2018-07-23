#include "stdafx.h"
#include "controlBase.h"
#include <utility>

ControlBase::ControlBase() :
	myWindow(nullptr)
{
}

ControlBase::ControlBase(ControlBase &&other) :
	myWindow(other.myWindow)
{
	other.myWindow = nullptr;
	if (myWindow)
		SetWindowLongPtr(myWindow, GWLP_USERDATA, (LONG_PTR)this);
}

ControlBase& ControlBase::operator=(ControlBase &&other) {
	if (myWindow) {
		SetWindowLongPtr(myWindow, GWLP_USERDATA, 0);
		dispose();
	}
	
	myWindow = other.myWindow;
	other.myWindow = nullptr;
	if (myWindow)
		SetWindowLongPtr(myWindow, GWLP_USERDATA, (LONG_PTR)this);
	return *this;
}

ControlBase::~ControlBase() {
	if (myWindow) {
		SetWindowLongPtr(myWindow, GWLP_USERDATA, 0);
		dispose();
	}
}

LRESULT CALLBACK ControlBase::windowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	ControlBase *thisControl = nullptr;

	if (msg == WM_NCCREATE) {
		CREATESTRUCT *csParam = (CREATESTRUCT*)lParam;
		thisControl = (ControlBase*)csParam->lpCreateParams;
		SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)thisControl);

		thisControl->myWindow = hWnd;
	}
	else
		thisControl = (ControlBase*)GetWindowLongPtr(hWnd, GWLP_USERDATA);

	if (thisControl)
		return thisControl->handleMessage(msg, wParam, lParam);
	else
		return DefWindowProc(hWnd, msg, wParam, lParam);
}

bool ControlBase::create(LPCWSTR sWindowName,
	int x, int y,
	int width, int height,
	HWND hParent,
	int ctrlId,
	DWORD dwStyle,
	DWORD dwExStyle)
{
	if (myWindow)
		return false;

	WNDCLASS wc = { 0 };
	wc.lpfnWndProc = &ControlBase::windowProc;
	wc.hInstance = GetModuleHandle(nullptr);
	wc.lpszClassName = className();
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

	RegisterClass(&wc);

	myWindow = CreateWindowEx(dwExStyle, wc.lpszClassName, sWindowName, dwStyle, x, y,
		width, height, hParent, (HMENU)ctrlId, wc.hInstance, this);
	return myWindow != nullptr;
}

void ControlBase::dispose() {
	DestroyWindow(myWindow);
	myWindow = nullptr;
}

int ControlBase::getControlWidth() const {
	RECT rcWindow;
	GetClientRect(myWindow, &rcWindow);
	return rcWindow.right;
}

int ControlBase::getControlHeight() const {
	RECT rcWindow;
	GetClientRect(myWindow, &rcWindow);
	return rcWindow.bottom;
}

void ControlBase::setClassCursor(HCURSOR hCursor) {
	if (myWindow)
		SetClassLongPtr(myWindow, GCLP_HCURSOR, (LONG_PTR)hCursor);
}
