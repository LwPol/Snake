#pragma once

#include "staticObject.h"

class BasicTile : public StaticObject  {
	public:
		///ctors
		BasicTile(int x, int y);

		/// public methods
		void draw(HDC hdc, int fieldWidth, int fieldHeight) override;
};