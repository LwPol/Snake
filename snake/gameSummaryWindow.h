#pragma once

#include "stdafx.h"

class GameSummaryWindow : public ControlBase {
	private:
		int score;
		int timeInSeconds;

		SmartHandle<HFONT> hfWndFont;
	public:
		GameSummaryWindow(int score = 0, int timeInSeconds = 0);
		GameSummaryWindow(const GameSummaryWindow &other);

		GameSummaryWindow& operator=(const GameSummaryWindow &other);

		LPCWSTR className() const override {
			return L"game_summary_window";
		}

		LRESULT handleMessage(UINT msg, WPARAM wParam, LPARAM lParam) override;

		static constexpr int width = 300, height = 300;
};