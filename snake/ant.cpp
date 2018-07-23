#include "stdafx.h"
#include "ant.h"
#include "snake.h"

SmartHandle<HBITMAP> Ant::hbmAnt[static_cast<int>(Orientation::COUNT)];
SmartHandle<HBITMAP> Ant::hbmAntMask[static_cast<int>(Orientation::COUNT)];

Ant::Ant(int x, int y, Orientation move) :
	DynamicObject(x, y),
	movingDirection(move)
{
}

Ant::Ant(int x, int y, Orientation move, const boost::function<void(Ant*)> &disposedHandler) :
	DynamicObject(x, y, disposedHandler),
	movingDirection(move)
{
}

Ant::~Ant() {
	Disposed(this);
}

void Ant::draw(HDC hdc, int fieldWidth, int fieldHeight) {
	constexpr int antBitmapWidth = 30, antBitmapHeight = 30;
	SmartHandle<HDC> hdcTemp(CreateCompatibleDC(nullptr));
	hdcTemp.setBitmap(hbmAntMask[static_cast<int>(movingDirection)]);
	BitBlt(hdc, getPosX() * fieldWidth, getPosY() * fieldHeight, antBitmapWidth, antBitmapHeight, hdcTemp, 0, 0, SRCAND);
	hdcTemp.setBitmap(hbmAntMask[static_cast<int>(movingDirection)]);

	hdcTemp.setBitmap(hbmAnt[static_cast<int>(movingDirection)]);
	BitBlt(hdc, getPosX() * fieldWidth, getPosY() * fieldHeight, antBitmapWidth, antBitmapHeight, hdcTemp, 0, 0, SRCPAINT);
	hdcTemp.setBitmap(hbmAnt[static_cast<int>(movingDirection)]);
}

void Ant::update(int timePassed) {
	DynamicObjectBase::update(timePassed);
	if (time >= oneTileTime) {
		time -= oneTileTime;

		POINT ptNewLocation = { getPosX(), getPosY() };
		getNextLocationFromOrientation(movingDirection, &ptNewLocation);
		setPosition(ptNewLocation.x, ptNewLocation.y);
	}
}

bool Ant::onCollision(ObjectOnMap *other) {
	if (dynamic_cast<Snake*>(other)) {
		Snake &dangerForAnt = static_cast<Snake&>(*other);
		if (dangerForAnt.getPosX() == getPosX() && dangerForAnt.getPosY() == getPosY()) {
			if (dangerForAnt.getHeadOrientation() == getOppositeDirection(movingDirection))
				return false;
		}
	}
	movingDirection = getOppositeDirection(movingDirection);
	return false;
}

bool Ant::loadAntBitmaps(const std::string &baseBitmapPath) {
	hbmAnt[0] = reinterpret_cast<HBITMAP>(LoadImageA(nullptr, baseBitmapPath.c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE));
	if (!hbmAnt)
		return false;
	for (int i = 1; i < static_cast<int>(Orientation::COUNT); ++i) {
		hbmAnt[i] = createRotatedBitmap(hbmAnt[0], (RotationType)(i - 1));
		hbmAntMask[i] = createBitmapMask(hbmAnt[i], RGB(255, 0, 255));
		if (!hbmAnt[i] || !hbmAntMask[i])
			return false;
	}
	hbmAntMask[0] = createBitmapMask(hbmAnt[0], RGB(255, 0, 255));
	if (!hbmAntMask[0])
		return false;

	return true;
}

void Ant::unloadAntBitmaps() {
	for (int i = 0; i < _countof(hbmAnt); ++i) {
		hbmAnt[i] = nullptr;
		hbmAntMask[i] = nullptr;
	}
}
