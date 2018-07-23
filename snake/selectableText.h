#pragma once

#include "clickableText.h"

class SelectableText : public ClickableText {
	private:
		bool bSelected = false;
		COLORREF crSelected;
	public:
		SelectableText(const std::wstring &sText, COLORREF normal = RGB(0, 0, 0),
			COLORREF highlight = RGB(255, 255, 255), COLORREF selected = RGB(0, 0, 200));
		SelectableText(std::wstring &&sText, COLORREF normal = RGB(0, 0, 0),
			COLORREF highlight = RGB(255, 255, 255), COLORREF selected = RGB(0, 0, 200));

		LRESULT handleMessage(UINT, WPARAM, LPARAM) override;
		LPCWSTR className() const override {
			return L"selectable_text";
		}

		void select() { bSelected = true; RedrawWindow(myWindow, nullptr, nullptr, RDW_INVALIDATE); }
		void unselect() { bSelected = false; RedrawWindow(myWindow, nullptr, nullptr, RDW_INVALIDATE); }
		bool isSelected() const { return bSelected; }
};