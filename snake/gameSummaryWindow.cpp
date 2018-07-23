#include "stdafx.h"
#include "gameSummaryWindow.h"
#include "startingMenu.h"

#pragma comment(lib, "msimg32.lib")

GameSummaryWindow::GameSummaryWindow(int score, int timeInSeconds) :
	score(score),
	timeInSeconds(timeInSeconds),
	hfWndFont(CreateFont(0, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, 0, L"Arial")) {}

GameSummaryWindow::GameSummaryWindow(const GameSummaryWindow &other) :
	GameSummaryWindow(other.score, other.timeInSeconds) {}

GameSummaryWindow& GameSummaryWindow::operator=(const GameSummaryWindow &other) {
	score = other.score;
	timeInSeconds = other.timeInSeconds;
	return *this;
}

LRESULT GameSummaryWindow::handleMessage(UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
		case WM_CREATE:
			SetWindowPos(myWindow, nullptr, 0, 0, width, height, SWP_NOZORDER | SWP_NOMOVE);
			SetFocus(myWindow);
			break;
		case WM_PAINT: {
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(myWindow, &ps);
			
			COLORREF *crBackground = new COLORREF[width * height];
			for (int i = 0; i < width * height; ++i) {
				crBackground[i] = RGB(255, 128, 0);
				crBackground[i] |= 128 << 24;
			}
			HBITMAP hbmBkground = CreateBitmap(width, height, 1, 32, crBackground);
			delete[] crBackground;

			HDC hdcTemp = CreateCompatibleDC(hdc);
			SelectObject(hdcTemp, hbmBkground);
			BLENDFUNCTION bfFunc = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
			AlphaBlend(hdc, 0, 0, width, height, hdcTemp, 0, 0, width, height, bfFunc);
			DeleteDC(hdcTemp);
			DeleteObject(hbmBkground);

			WCHAR buffer[128];
			SIZE textSize;
			SelectObject(hdc, hfWndFont);
			SetBkMode(hdc, TRANSPARENT);
			wsprintf(buffer, L"Uzyska³eœ wynik: %d pkt", score);
			GetTextExtentPoint32(hdc, buffer, lstrlen(buffer), &textSize);
			TextOut(hdc, (width - textSize.cx) / 2, 70, buffer, lstrlen(buffer));

			int minutes = timeInSeconds / 60;
			int secondsRemainer = timeInSeconds - 60 * minutes;
			WCHAR sSeconds[3];
			wsprintf(sSeconds, L"%2d", secondsRemainer);
			if (sSeconds[0] == L' ')
				sSeconds[0] = L'0';
			wsprintf(buffer, L"Czas gry: %d:%.2s", minutes, sSeconds);
			GetTextExtentPoint32(hdc, buffer, lstrlen(buffer), &textSize);
			TextOut(hdc, (width - textSize.cx) / 2, 160, buffer, lstrlen(buffer));

			LPCWSTR sContinueInfo = L"Naciœnij ENTER aby kontynuowaæ";
			GetTextExtentPoint32(hdc, sContinueInfo, lstrlen(sContinueInfo), &textSize);
			TextOut(hdc, (width - textSize.cx) / 2, 270, sContinueInfo, lstrlen(sContinueInfo));

			EndPaint(myWindow, &ps);
			break;
		}
		case WM_KEYUP:
			switch (wParam) {
				case VK_RETURN:
					Application::setNewWindowProc<StartingMenu>();
					break;
			}
			break;
		default:
			return DefWindowProc(myWindow, msg, wParam, lParam);
	}
	return 0;
}
