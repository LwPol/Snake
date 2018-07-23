#include "stdafx.h"
#include "basicTile.h"

//
//	CONSTRUCTOR: BasicTile(int, int)
//
//	COMMENT: delegates to ObjectOnMap(int, int) ctor
//
BasicTile::BasicTile(int x, int y) :
	StaticObject(x, y) {}

//
//	FUNCTION: draw(HDC, int, int)
//
//	COMMENT: overrides ObjectOnMap::draw(HDC, int, int)
//
void BasicTile::draw(HDC hdc, int fieldWidth, int fieldHeight) {
	RECT rcArea = {
		fieldWidth * getPosX(),
		fieldHeight * getPosY(),
		fieldWidth * (getPosX() + 1),
		fieldHeight * (getPosY() + 1)
	};
	FillRect(hdc, &rcArea, reinterpret_cast<HBRUSH>(GetStockObject(GRAY_BRUSH)));
	DrawEdge(hdc, &rcArea, BDR_RAISEDOUTER, BF_RECT);
}
