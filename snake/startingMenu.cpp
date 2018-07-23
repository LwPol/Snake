#include "stdafx.h"
#include "startingMenu.h"
#include "snakeWndProc.h"

StartingMenu::StartingMenu() :
	hfMenuFont(CreateFont(40, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, 0, L"Arial"))
{
	hbrBackground = CreateSolidBrush(RGB(0, 128, 50));
	if (!hbrBackground)
		throw StartingMenuInitException();

	if (Application::hWnd())
		initialize(Application::hWnd());
}

void StartingMenu::getMenuEntriesSize(SIZE *entriesSize) {
	if (menuEntries.empty()) {
		entriesSize->cx = entriesSize->cy = 0;
		return;
	}

	int width = -1;
	int height = 0;
	for (auto &entry : menuEntries) {
		RECT rcWnd;
		GetClientRect(entry.window(), &rcWnd);
		if (rcWnd.right > width)
			width = rcWnd.right;
		height += rcWnd.bottom;
	}
	height += (menuEntries.size() - 1) * menuEntriesVertDist;

	entriesSize->cx = width;
	entriesSize->cy = height;
}

LRESULT StartingMenu::wndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
		case WM_CREATE:
			initialize(hWnd);
			break;
		case WM_PAINT: {
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);
			RECT rcWnd;
			GetClientRect(hWnd, &rcWnd);
			FillRect(hdc, &rcWnd, hbrBackground);
			EndPaint(hWnd, &ps);
			break;
		}
		case WM_COMMAND:
			switch ((MenuEntry)LOWORD(wParam)) {
				case MenuEntry::PLAY:
					Application::setNewWindowProc<SnakeWndProc>();
					break;
				case MenuEntry::QUIT:
					Application::setNewWindowProc<void>();
					PostMessage(Application::hWnd(), WM_CLOSE, 0, 0);
					break;
			}
			break;
		case WM_DESTROY:
			Application::setNewWindowProc<void>();
			break;
		default:
			return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

void StartingMenu::initialize(HWND hWnd) {
	menuEntries.push_back(ClickableText(L"Graj"));
	menuEntries.push_back(ClickableText(L"Wyjdü"));

	menuEntries[0].create(nullptr, 0, 0, 0, 0, hWnd, static_cast<int>(MenuEntry::PLAY));
	menuEntries[1].create(nullptr, 0, 0, 0, 0, hWnd, static_cast<int>(MenuEntry::QUIT));

	for (auto &entry : menuEntries) {
		SendMessage(entry.window(), WM_SETFONT, reinterpret_cast<WPARAM>(static_cast<HFONT>(hfMenuFont)), FALSE);
		entry.adjustSize();
	}

	SIZE sz;
	getMenuEntriesSize(&sz);

	RECT rcWnd;
	GetClientRect(hWnd, &rcWnd);

	int currentHorz;
	int currentVert = (rcWnd.bottom - sz.cy) / 2;
	for (int i = 0; i < menuEntries.size(); ++i) {
		currentHorz = (rcWnd.right - menuEntries[i].getControlWidth()) / 2;
		SetWindowPos(menuEntries[i].window(), nullptr, currentHorz, currentVert, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
		ShowWindow(menuEntries[i].window(), SW_SHOW);
		currentVert += menuEntries[i].getControlHeight() + menuEntriesVertDist;
	}

	RedrawWindow(hWnd, nullptr, nullptr, RDW_INVALIDATE);
}
