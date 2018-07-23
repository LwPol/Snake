#include "stdafx.h"
#include "clickableText.h"

void ClickableText::adjustSize() {
	HDC hdcTemp = CreateCompatibleDC(nullptr);
	if (!hdcTemp)
		return;

	SelectObject(hdcTemp, hfText);
	SIZE preferredSize;
	GetTextExtentPoint32(hdcTemp, sText.c_str(), sText.size(), &preferredSize);
	SetWindowPos(myWindow, nullptr, 0, 0, preferredSize.cx, preferredSize.cy, SWP_NOZORDER | SWP_NOMOVE);
	DeleteDC(hdcTemp);
}

LRESULT ClickableText::handleMessage(UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
		case WM_PAINT: {
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(myWindow, &ps);

			SetBkMode(hdc, TRANSPARENT);
			if (bHighlight)
				SetTextColor(hdc, crHighlight);
			else
				SetTextColor(hdc, crNormal);
			SelectObject(hdc, hfText);

			SIZE size;
			GetTextExtentPoint32(hdc, sText.c_str(), sText.size(), &size);
			RECT rcWnd;
			GetClientRect(myWindow, &rcWnd);

			TextOut(hdc, (rcWnd.right - size.cx) / 2, (rcWnd.bottom - size.cy) / 2, sText.c_str(), sText.size());

			EndPaint(myWindow, &ps);
			break;
		}
		case WM_MOUSEMOVE: {
			if (GetCapture() != myWindow) {
				bHighlight = true;
				SetCapture(myWindow);
				RedrawWindow(myWindow, nullptr, nullptr, RDW_INVALIDATE);
				break;
			}

			RECT rcWnd;
			GetClientRect(myWindow, &rcWnd);
			if (LOWORD(lParam) >= rcWnd.right || HIWORD(lParam) >= rcWnd.bottom) {
				bHighlight = false;
				RedrawWindow(myWindow, nullptr, nullptr, RDW_INVALIDATE);
				ReleaseCapture();
			}
			break;
		}
		case WM_LBUTTONUP: {
			SendMessage(GetParent(myWindow),
				WM_COMMAND,
				GetWindowLongPtr(myWindow, GWLP_ID),
				reinterpret_cast<LPARAM>(myWindow));
			break;
		}
		case WM_SETFONT: {
			HFONT hf = reinterpret_cast<HFONT>(wParam);
			if (!hf)
				hf = (HFONT)GetStockObject(SYSTEM_FONT);
			hfText = hf;
			if (lParam)
				RedrawWindow(myWindow, nullptr, nullptr, RDW_INVALIDATE);
			break;
		}
		case WM_GETFONT: {
			if (hfText == static_cast<HFONT>(GetStockObject(SYSTEM_FONT)))
				return reinterpret_cast<LRESULT>(nullptr);
			else
				return reinterpret_cast<LRESULT>(hfText);
		}
		default:
			return DefWindowProc(myWindow, msg, wParam, lParam);
	}
	return 0;
}
