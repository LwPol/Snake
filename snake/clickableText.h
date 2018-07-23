#pragma once

#include "stdafx.h"
#include <string>
#include <utility>

class ClickableText : public ControlBase {
	private:
		std::wstring sText;
		COLORREF crNormal;
		COLORREF crHighlight;

		bool bHighlight = false;

	protected:
		HFONT hfText;
	public:
		ClickableText(const std::wstring &sText, COLORREF normal = RGB(0, 0, 0), COLORREF highlight = RGB(255, 255, 255)) :
			sText(sText),
			crNormal(normal),
			crHighlight(highlight),
			hfText(static_cast<HFONT>(GetStockObject(SYSTEM_FONT)))
		{
		}
		ClickableText(std::wstring &&sText, COLORREF normal = RGB(0, 0, 0), COLORREF highlight = RGB(255, 255, 255)) :
			sText(std::move(sText)),
			crNormal(normal),
			crHighlight(highlight),
			hfText(static_cast<HFONT>(GetStockObject(SYSTEM_FONT)))
		{
		}
		ClickableText(ClickableText &&other) :
			ControlBase(std::move(other)),
			sText(std::move(other.sText)),
			crNormal(other.crNormal),
			crHighlight(other.crHighlight),
			hfText(other.hfText)
		{
		}

		void adjustSize();

		LPCWSTR className() const override { return L"clickable_text_snake"; }
		LRESULT handleMessage(UINT, WPARAM, LPARAM) override;

		const std::wstring& getText() const { return sText; }
		template <class T>
		void setText(T &&text) { sText = std::forward<T>(text); }
};