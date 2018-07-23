#include "stdafx.h"
#include "objectOnMap.h"

//
//	CONSTRUCTOR: ObjectOnMap(int, int)
//
//	PARAMS:
//		int x - x coordinate of the object
//		int y - y coordinate of the object
//
ObjectOnMap::ObjectOnMap(int x, int y) :
	posX(x), posY(y) {}

//
//	FUNCTION: createRotatedBitmap(HBITMAP, ObjectOnMap::RotationType)
//
//	PURPOSE:
//		Creates new bitmap by rotating of existing one
//
//	PARAMS:
//		HBITMAP hbmSource - bitmap to rotate; bitmap must have 24 or 32 bpp
//		RotationType type - type of rotation
//
//	RETURNS: HBITMAP
//		On success returns handle to newly created rotated bitmap, or NULL if fails
//
HBITMAP ObjectOnMap::createRotatedBitmap(HBITMAP hbmSource, RotationType type) {
	if (type < RotationType::LEFT_90 || type > RotationType::RIGHT_90 || hbmSource == nullptr) {
		SetLastError(ERROR_BAD_ARGUMENTS);
		return nullptr;
	}
	
	BITMAP bmInfo;
	GetObject(hbmSource, sizeof(BITMAP), &bmInfo);

	if (bmInfo.bmBitsPixel != 24 && bmInfo.bmBitsPixel != 32) {
		SetLastError(ERROR_INVALID_PARAMETER);
		return nullptr;
	}

	BITMAPINFOHEADER bih;
	memset(&bih, 0, sizeof(BITMAPINFOHEADER));
	bih.biSize = sizeof(BITMAPINFOHEADER);
	bih.biWidth = bmInfo.bmWidth;
	bih.biHeight = -bmInfo.bmHeight;
	bih.biBitCount = bmInfo.bmBitsPixel;
	bih.biPlanes = bmInfo.bmPlanes;
	bih.biCompression = BI_RGB;

	// determine needed buffer size
	int orgRowSize = ((bmInfo.bmWidth * bmInfo.bmBitsPixel + 31) / 32) * 4;
	int orgBufferSize = orgRowSize * bmInfo.bmHeight;
	int newRowSize, newBufferSize;
	if (type == RotationType::ROT_180) {
		newRowSize = orgRowSize;
		newBufferSize = orgBufferSize;
	}
	else {
		newRowSize = ((bmInfo.bmHeight * bmInfo.bmBitsPixel + 31) / 32) * 4;
		newBufferSize = newRowSize * bmInfo.bmWidth;
	}

	// allocate memory for buffers
	char *orginalBitmap = new (std::nothrow) char[orgBufferSize];
	char *newBitmap = new (std::nothrow) char[newBufferSize];
	if (!orginalBitmap || !newBitmap) {
		delete[] orginalBitmap;
		delete[] newBitmap;
		SetLastError(ERROR_OUTOFMEMORY);
		return nullptr;
	}

	// copy bitmaps bits to a buffer
	HDC hdcTemp = CreateCompatibleDC(nullptr);
	int retDIB = GetDIBits(hdcTemp, hbmSource, 0, bmInfo.bmHeight, orginalBitmap, (BITMAPINFO*)&bih, DIB_RGB_COLORS);
	DeleteDC(hdcTemp);
	if (retDIB == 0 || retDIB == ERROR_INVALID_PARAMETER) {
		delete[] orginalBitmap;
		delete[] newBitmap;
		return nullptr;
	}

	// process image
	for (int i = 0; i < bmInfo.bmWidth; ++i) {
		for (int j = 0; j < bmInfo.bmHeight; ++j) {
			int orgIndex = j * newRowSize + bmInfo.bmBitsPixel / 8 * i;
			int x, y;
			switch (type) {
				case RotationType::LEFT_90:
					x = j;
					y = bmInfo.bmWidth - 1 - i;
					break;
				case RotationType::ROT_180:
					x = bmInfo.bmWidth - 1 - i;
					y = bmInfo.bmHeight - 1 - j;
					break;
				case RotationType::RIGHT_90:
					x = bmInfo.bmHeight - 1 - j;
					y = i;
					break;
			}

			int newIndex = y * newRowSize + bmInfo.bmBitsPixel / 8 * x;
			for (int k = 0; k < bmInfo.bmBitsPixel / 8; ++k)
				newBitmap[newIndex++] = orginalBitmap[orgIndex++];
		}
	}

	// create bitmap and release memory
	delete[] orginalBitmap;
	HBITMAP hbmRet = CreateBitmap(type == RotationType::ROT_180 ? bmInfo.bmWidth : bmInfo.bmHeight,
		type == RotationType::ROT_180 ? bmInfo.bmHeight : bmInfo.bmWidth,
		bmInfo.bmPlanes,
		bmInfo.bmBitsPixel,
		newBitmap);
	delete[] newBitmap;
	return hbmRet;
}

//
//	FUNCTION: createBitmapMask(HBITMAP, COLORREF)
//
//	PURPOSE:
//		Creates bitmap mask for given bitmap and transparent color
//
//	PARAMS:
//		HBITMAP hbmSource - bitmap to create mask for; note that transparent color of this bitmap is changed into black after function returns
//		COLORREF crTransparent - color to be transparent
//
//	RETURNS: HBITMAP
//		Returns handle to the created mask, or NULL if fails.
//
HBITMAP ObjectOnMap::createBitmapMask(HBITMAP hbmSource, COLORREF crTransparent) {
	// check if bitmap handle is not NULL
	if (!hbmSource) {
		SetLastError(ERROR_BAD_ARGUMENTS);
		return nullptr;
	}

	// create DCs to hold the bitmap and its mask
	HDC hdcSource = CreateCompatibleDC(nullptr);
	HDC hdcMask = CreateCompatibleDC(nullptr);
	if (!hdcSource || !hdcMask) {
		DeleteDC(hdcSource);
		DeleteDC(hdcMask);
		return nullptr;
	}

	// retrieve bitmap's size
	BITMAP bmInfo;
	GetObject(hbmSource, sizeof(BITMAP), &bmInfo);

	// create mask and select bitmaps into DCs
	HBITMAP hbmMask = CreateBitmap(bmInfo.bmWidth, bmInfo.bmHeight, 1, 1, nullptr);
	if (!SelectObject(hdcSource, hbmSource)) {
		DeleteDC(hdcSource);
		DeleteDC(hdcMask);
		DeleteObject(hbmMask);
		return nullptr;
	}
	SelectObject(hdcMask, hbmMask);

	// perform blit operations on bitmaps
	SetBkColor(hdcSource, crTransparent);
	BitBlt(hdcMask, 0, 0, bmInfo.bmWidth, bmInfo.bmHeight, hdcSource, 0, 0, SRCCOPY);
	BitBlt(hdcSource, 0, 0, bmInfo.bmWidth, bmInfo.bmHeight, hdcMask, 0, 0, SRCINVERT);

	// cleanup and return
	DeleteDC(hdcSource);
	DeleteDC(hdcMask);

	return hbmMask;
}

//
//	FUNCTION: getOppositeDirection(ObjectOnMap::Orientation)
//
//	PURPOSE:
//		Retrieves direction that is opposite to a given one (e.g. for UP returns DOWN)
//
//	PARAMS:
//		Orientation dir - direction for which opposite is retrived
//
//	RETURNS: ObjectOnMap::Orientation
//		Returns opposite direction to dir
//
ObjectOnMap::Orientation ObjectOnMap::getOppositeDirection(Orientation dir) {
	return static_cast<Orientation>((static_cast<int>(dir) + 2) % static_cast<int>(Orientation::COUNT));
}

//
//	FUNCTION: getNextDirection(ObjectOnMap::Orientation, bool)
//
//	PURPOSE:
//		Retrieves next counter-clockwise or clockwise direction to given (e.g. for UP and true returns LEFT)
//
//	PARAMS:
//		Orientation dir - direction for which next one is retrived
//		bool bCounterClockWise - value indicating whether direction should be searched CCW or CW (true for CCW)
//
//	RETURNS: ObjectOnMap::Orienatation
//		Returns next direction to given
//
ObjectOnMap::Orientation ObjectOnMap::getNextDirection(Orientation dir, bool bCounterClockWise) {
	return bCounterClockWise ?
		static_cast<Orientation>((static_cast<int>(dir) + 1) % static_cast<int>(Orientation::COUNT)) :
		static_cast<Orientation>((static_cast<int>(dir) + 3) % static_cast<int>(Orientation::COUNT));
}

//
//	FUNCTION: getNextLocationFromOrientation(ObjectOnMap::Orientation, POINT*)
//
//	PURPOSE:
//		Calculates position after moving one unit in given direction from given point
//
//	PARAMS:
//		Orientation direction - direction to move
//		POINT* ptCoords - pointer to POINT struct specifing starting and retriving ending location; cannot be NULL
//
void ObjectOnMap::getNextLocationFromOrientation(Orientation direction, POINT *ptCoords) {
	switch (direction) {
		case Orientation::UP:
			--ptCoords->y;
			break;
		case Orientation::LEFT:
			--ptCoords->x;
			break;
		case Orientation::DOWN:
			++ptCoords->y;
			break;
		case Orientation::RIGHT:
			++ptCoords->x;
			break;
		default:
			throw std::invalid_argument("ObjectOnMap::getNextLocationFromOrientation: Invalid enum value");
	}
}
