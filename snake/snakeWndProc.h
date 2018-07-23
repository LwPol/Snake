#pragma once

#include "stdafx.h"
#include "snakeMap.h"
#include "basicTile.h"
#include "basicFood.h"
#include "ant.h"
#include "objectsOnMapManager.h"
#include "scoreDisplay.h"
#include "cursorVisibilityManager.h"

#include "gameSummaryWindow.h"

class SnakeWndProc : public WindowProc {
	private:
		SnakeMap smMap;

		ObjectsOnMapManager oommManager;

		ScoreDisplay sdDisp;

		SmartHandle<HDC> hdcMem;

		DWORD dwLastTick;

		GameSummaryWindow gswSummary;

		CursorVisibilityManager cvmCursor;

		bool bTimersStarted = false;

		void timerProc(DWORD dwTime);

		template <class T>
		void snakeCollisionHandler(T *collisionObject, Snake *snake);
		template <>
		void snakeCollisionHandler(StaticObject *collisionObject, Snake *snake);
		template <>
		void snakeCollisionHandler(Snake *collisionObject, Snake *snake);
		template <>
		void snakeCollisionHandler(BasicFood *collisionObject, Snake *snake);
		template <>
		void snakeCollisionHandler(Ant *collisionObject, Snake *snake);

		template <class T>
		void attachSnakeCollisionHandler();
		template <class T>
		void detachSnakeCollisionHandler();

		void initCollisionHandlers();
		void detachCollisionHandlers();

		void snakeNewElementAddedHandler(Snake *snake);

		void adjustWindowSize(HWND hWnd);
		void initializeGame(HWND hWnd);
	public:
		SnakeWndProc();

		~SnakeWndProc();

		LRESULT wndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) override;

		void startTimer(HWND hWnd);

		void endGame();
};

template <class T>
void SnakeWndProc::snakeCollisionHandler(T *collisionObject, Snake *snake) {
	static_assert(false, "This template must be specialized");
}

template <>
void SnakeWndProc::snakeCollisionHandler(StaticObject *collisionObject, Snake *snake) {
	endGame();
}

template <>
void SnakeWndProc::snakeCollisionHandler(Snake *collisionObject, Snake *snake) {
	endGame();
}

template <>
void SnakeWndProc::snakeCollisionHandler(BasicFood *collisionObject, Snake *snake) {
	snake->addElement();
	smMap.deleteObject(collisionObject);
}

template <>
void SnakeWndProc::snakeCollisionHandler(Ant *collisionObject, Snake *snake) {
	snake->addElement();
	smMap.deleteObject(collisionObject);
}

template <class T>
void SnakeWndProc::attachSnakeCollisionHandler() {
	T::SnakeCollision.connect(boost::bind(&SnakeWndProc::snakeCollisionHandler<T>, this, _1, _2));
}

template <class T>
void SnakeWndProc::detachSnakeCollisionHandler() {
	T::SnakeCollision.disconnect(boost::bind(&SnakeWndProc::snakeCollisionHandler<T>, this, _1, _2));
}