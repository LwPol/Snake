#include "stdafx.h"
#include "basicFood.h"

SmartHandle<HBITMAP> BasicFood::hbmFood;
SmartHandle<HBITMAP> BasicFood::hbmFoodMask;

//
//	CONSTRUCTOR: BasicFood(int, int)
//
//	COMMENT: delegates to DynamicObjectBase(int, int)
//
//BasicFood::BasicFood(int x, int y) :
//	DynamicObjectBase(x, y)
//{
//	//Created(this);
//}

/// doc!
BasicFood::~BasicFood() {
	Disposed(this);
}

bool BasicFood::loadBitmaps(const std::string &bitmapPath) {
	hbmFood = reinterpret_cast<HBITMAP>(LoadImageA(nullptr, bitmapPath.c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE));
	if (!hbmFood)
		return false;
	hbmFoodMask = createBitmapMask(hbmFood, RGB(255, 0, 255));
	if (!hbmFoodMask) {
		hbmFood = nullptr;
		return false;
	}
	return true;
}

//
//	FUNCTION: unloadBitmaps()
//
//	PURPOSE:
//		Frees resources used by class static bitmaps
//
void BasicFood::unloadBitmaps() {
	hbmFood = nullptr;
	hbmFoodMask = nullptr;
}

//
//	FUNCTION: draw(HDC, int, int)
//
//	COMMENT: overrides ObjectOnMap::draw(HDC, int, int)
//
void BasicFood::draw(HDC hdc, int fieldWidth, int fieldHeight) {
	RECT rcArea = {
		fieldWidth * getPosX(),
		fieldHeight * getPosY(),
		fieldWidth * (getPosX() + 1),
		fieldHeight * (getPosY() + 1)
	};

	int stretchBlt = GetStretchBltMode(hdc);
	SetStretchBltMode(hdc, HALFTONE);

	BITMAP bmFood;
	GetObject(hbmFood, sizeof(bmFood), &bmFood);

	SmartHandle<HDC> hdcMem(CreateCompatibleDC(nullptr));

	hdcMem.setBitmap(hbmFoodMask);
	StretchBlt(hdc, rcArea.left, rcArea.top, fieldWidth, fieldHeight,
		hdcMem, 0, 0, bmFood.bmWidth, bmFood.bmHeight, SRCAND);
	hdcMem.setBitmap(hbmFoodMask);

	hdcMem.setBitmap(hbmFood);
	StretchBlt(hdc, rcArea.left, rcArea.top, fieldWidth, fieldHeight,
		hdcMem, 0, 0, bmFood.bmWidth, bmFood.bmHeight, SRCPAINT);
	hdcMem.setBitmap(hbmFood);

	SetStretchBltMode(hdc, stretchBlt);
}