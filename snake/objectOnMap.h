#pragma once

#include "stdafx.h"

class Snake;

class ObjectOnMap {
	public:
		// types
		enum class RotationType {
			LEFT_90, ROT_180, RIGHT_90
		};

		enum class Orientation {
			UP, LEFT, DOWN, RIGHT, COUNT
		};

		struct StaticObjectTag {};
		struct DynamicObjectTag {};
	private:
		// private fields
		int posX, posY;
	public:
		// ctors
		ObjectOnMap(int x, int y);

		// dtor
		virtual ~ObjectOnMap() = default;
		
		// public methods
		static HBITMAP createRotatedBitmap(HBITMAP hbmSource, RotationType type);
		static HBITMAP createBitmapMask(HBITMAP hbmSource, COLORREF crTransparent);

		static Orientation getOppositeDirection(Orientation dir);
		static Orientation getNextDirection(Orientation dir, bool bCounterClockWise);
		static void getNextLocationFromOrientation(Orientation direction, POINT *ptCoords);

		// accessors
		int getPosX() const { return posX; }
		int getPosY() const { return posY; }

		virtual void setPosition(int x, int y) { posX = x; posY = y; }

		// abstact methods
		virtual void draw(HDC hdc, int fieldWidth, int fieldHeight) = 0;

		virtual void raiseSnakeCollisionEvent(Snake *snake) = 0;
};