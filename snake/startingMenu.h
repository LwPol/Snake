#pragma once

#include "stdafx.h"
#include "clickableText.h"

class StartingMenu : public WindowProc {
	public:
		enum class MenuEntry {
			PLAY,
			QUIT
		};

	private:
		SmartHandle<HBRUSH> hbrBackground;
		SmartHandle<HFONT> hfMenuFont;

		std::vector<ClickableText> menuEntries;
		constexpr static int menuEntriesVertDist = 15;

		void initialize(HWND hWnd);
	public:
		StartingMenu();

		void getMenuEntriesSize(SIZE *entriesSize);

		LRESULT wndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) override;

		struct StartingMenuInitException : public std::exception {
			const char* what() const override {
				return "StartingMenu object initialization failed";
			}
		};
};