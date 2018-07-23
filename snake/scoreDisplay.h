#pragma once

#include "stdafx.h"

class ScoreDisplay {
	private:
		enum class TimeCounterState {
			NORMAL,
			SUSPENDED,
			ABORT,
			NOT_RUNNING
		};

		int width, heigth;
		SmartHandle<HDC> hdcDisplay;

		int score = 0;

		std::thread timeCounter;
		std::chrono::time_point<std::chrono::system_clock> lastUpdate;
		std::atomic<double> secondsInGame;
		TimeCounterState timeCounterState;
		std::mutex stateMutex;
		std::condition_variable stateSync;

		void updateTimer();
	public:
		ScoreDisplay(int width, int height);

		~ScoreDisplay();

		void draw();
		void render(HDC hdc, int x, int y);

		bool startTimer();
		void stopTimer();
		void pauseTimer();
		void unpauseTimer();

		int getWidth() const { return width; }
		int getHeight() const { return heigth; }
		void setSize(int width, int height);

		int getScore() const { return score; }
		void setScore(int value) { score = value; draw(); }
		void addScore(int points) { score += points; draw(); }

		int getSecondsInGame() const { return static_cast<int>(secondsInGame.load()); }

		struct ScoreDisplayException : public std::exception {
			enum Reason {
				CREATE_DC_FAIL,
				CREATE_FONT_FAIL
			} reason;

			ScoreDisplayException(Reason reason) :
				reason(reason)
			{
			}

			const char* what() const override {
				switch (reason) {
					case CREATE_DC_FAIL:
						return "Failed to create a device context for score displayer";
					case CREATE_FONT_FAIL:
						return "Failed to create a font for score displayer";
					default:
						return "Unknown exception";
				}
			}
		};
};