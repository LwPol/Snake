#include "stdafx.h"
#include "snakeMap.h"
#include "snakeMapLoader.h"
#include "snakeGameExceptions.h"

//
//	CONSTRUCTOR: SnakeMap(int, int)
//
//	COMMENT: creates an instance of empty map with specified size
//
//	PARAMS:
//		int width - width of the map, in tiles
//		int height - height of the map, in tiles
//	
SnakeMap::SnakeMap(int width, int height) :
	mapWidth(width),
	mapHeight(height),
	mapOverlay(width * height),
	snake(nullptr)
{
	// create background brush
	SmartHandle<HBITMAP> hbmBackground = reinterpret_cast<HBITMAP>(LoadImage(nullptr, L"graphics\\grass.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE));
	if (!hbmBackground) {
		if (GetLastError() == ERROR_FILE_NOT_FOUND)
			throw BitmapLoadFail("File not found: graphics\\grass.bmp");
		else
			throw BitmapLoadFail("Cannot load file graphics\\grass.bmp", GetLastError());
	}
	hbrBackground = CreatePatternBrush(hbmBackground);
}

SnakeMap::SnakeMap(const std::string &sFilePath) : SnakeMap(SnakeMapLoader::loadMapFromFile(sFilePath))
{
}

SnakeMap::SnakeMap(SnakeMap &&other) :
	mapWidth(other.mapWidth),
	mapHeight(other.mapHeight),
	snake(nullptr),
	hbrBackground(std::move(other.hbrBackground)),
	mapOverlay(std::move(other.mapOverlay)),
	staticObjsCollection(std::move(other.staticObjsCollection)),
	dynamicObjsCollection(std::move(other.dynamicObjsCollection))
{
	if (other.snake) {
		for (auto &obj : mapOverlay) {
			if (obj == other.snake)
				obj = nullptr;
		}
	}
}

SnakeMap& SnakeMap::operator=(SnakeMap &&other) {
	mapWidth = other.mapWidth;
	mapHeight = other.mapHeight;

	hbrBackground = std::move(other.hbrBackground);

	mapOverlay = std::move(other.mapOverlay);
	for (auto &sobj : staticObjsCollection)
		delete sobj;
	staticObjsCollection = std::move(other.staticObjsCollection);
	for (auto &dobj : dynamicObjsCollection) {
		dobj->detachDisposeHandlers();
		delete dobj;
	}
	dynamicObjsCollection = std::move(other.dynamicObjsCollection);
	if (other.snake) {
		for (auto &obj : mapOverlay) {
			if (obj == other.snake)
				obj = nullptr;
		}
	}
	delete snake;
	snake = nullptr;
	return *this;
}

//
//	DESTRUCTOR: ~SnakeMap()
//
//	COMMENT: frees resources used by GDI objects of the instance and destroys all objects on map
//
SnakeMap::~SnakeMap() {
	// release memory for objects on map
	for (auto &&sobj : staticObjsCollection)
		delete sobj;
	for (auto &&dobj : dynamicObjsCollection) {
		dobj->detachDisposeHandlers();
		delete dobj;
	}
	// delete snake
	delete snake;
}

//
//	FUNCTION: drawMap(HDC)
//
//	PURPOSE:
//		Draws map in specified device
//
//	PARAMS:
//		HDC hdc - handle to DC to draw onto
//
void SnakeMap::drawMap(HDC hdc) {
	// draw background
	RECT rcWnd = { 0, 0, mapWidth * fieldWidth, mapHeight * fieldHeight };
	FillRect(hdc, &rcWnd, hbrBackground);

	// draw static objects
	for (auto &sobj : staticObjsCollection)
		sobj->draw(hdc, fieldWidth, fieldHeight);

	// draw dynamic objects
	for (auto &dobj : dynamicObjsCollection)
		dobj->draw(hdc, fieldWidth, fieldHeight);

	// draw Snake
	if (snake)
		snake->draw(hdc, fieldWidth, fieldHeight, mapWidth, mapHeight);
}

//
//	FUNCTION: update(int)
//
//	PURPOSE:
//		Updates all dynamic objects on the map
//
//	PARAMS:
//		int timePassed - time that passed since last update
//
void SnakeMap::update(int timePassed) {
	for (auto &dobj : dynamicObjsCollection)
		dobj->update(timePassed);
	if (snake)
		snake->update(timePassed);
}

//
//	FUNCTION: getObject(int, int)
//
//	PURPOSE:
//		Retrives a pointer to an object located in point (x, y) on the map
//
//	PARAMS:
//		int x - x coordinate of the point
//		int y - y coordinate of the point
//
//	RETURNS: ObjectOnMap*
//		Returns pointer to object located in given point or NULL if there isn't any object in that location
//
ObjectOnMap* SnakeMap::getObject(int x, int y) {
	return mapOverlay[getMapOverlayIndex(x, y)];
}

//
//	FUNCTION: createSnake(int, int, ObjectOnMap::Orienatation)
//
//	PURPOSE:
//		Creates snake object in specified location
//
//	PARAMS:
//		int x - x coordinate of snake's head; must be in range [0, mapWidth)
//		int y - y coorindate of snake's head; must be in range [0, mapHeight)
//		ObjectOnMap::Orientation headOrientation - orientation of snake's head
//
//	RETURNS: bool
//		Returns true if snake was successfully created, and false otherwise. Function fails if there already are some objects
//			in location that snake would occupy or if arguments are invalid
//
bool SnakeMap::createSnake(int x, int y, ObjectOnMap::Orientation headOrientation) {
	if (headOrientation < Snake::Orientation::UP || headOrientation > Snake::Orientation::RIGHT)
		throw std::invalid_argument("SnakeMap::createSnake: Enum value out of range");

	if (x < 0 || x >= mapWidth || y < 0 || y >= mapHeight)
		throw std::invalid_argument("SnakeMap::createSnake: Coordinates lay beyound current map size");

	// check head position
	if (getObject(x, y))
		return false;
	
	// check middle part's position
	POINT ptMiddle = { x, y };
	ObjectOnMap::getNextLocationFromOrientation(ObjectOnMap::getOppositeDirection(headOrientation), &ptMiddle);
	fitCoordsInMap(&ptMiddle);
	if (getObject(ptMiddle.x, ptMiddle.y))
		return false;

	// check tail position
	POINT ptTail = ptMiddle;
	ObjectOnMap::getNextLocationFromOrientation(ObjectOnMap::getOppositeDirection(headOrientation), &ptTail);
	fitCoordsInMap(&ptTail);
	if (getObject(ptTail.x, ptTail.y))
		return false;

	snake = new Snake(x, y, headOrientation);
	snake->SnakeMoved.connect(boost::bind(&SnakeMap::snakeMovedHandler, this, _1, _2, _3, _4, _5));
	snake->NewElementAdded.connect(boost::bind(&SnakeMap::snakeNewElementAddedHandler, this, _1));
	
	// set snake's place in map overlay
	mapOverlay[getMapOverlayIndex(x, y)] = snake;
	mapOverlay[getMapOverlayIndex(ptMiddle.x, ptMiddle.y)] = snake;
	mapOverlay[getMapOverlayIndex(ptTail.x, ptTail.y)] = snake;
	return true;
}

/// doc!
void SnakeMap::deleteObject(DynamicObjectBase *obj) {
	bool bFound = false;
	for (auto it = dynamicObjsCollection.begin(); it != dynamicObjsCollection.end(); ++it) {
		if (*it == obj) {
			dynamicObjsCollection.erase(it);
			bFound = true;
			break;
		}
	}
	if (bFound) {
		mapOverlay[getMapOverlayIndex(obj->getPosX(), obj->getPosY())] = nullptr;
		delete obj;
	}
}

void SnakeMap::deleteObject(int x, int y) {
	if (x < 0 || x >= mapWidth || y < 0 || y >= mapHeight)
		return;

	ObjectOnMap *markedForDeletion = getObject(x, y);
	if (markedForDeletion == nullptr || markedForDeletion == snake)
		return;

	mapOverlay[getMapOverlayIndex(x, y)] = nullptr;
	bool bFound = false;
	for (auto it = staticObjsCollection.begin(); it != staticObjsCollection.end(); ++it) {
		if (*it == markedForDeletion) {
			staticObjsCollection.erase(it);
			bFound = true;
			break;
		}
	}
	if (!bFound) {
		for (auto it = dynamicObjsCollection.begin(); it != dynamicObjsCollection.end(); ++it) {
			if (*it == markedForDeletion) {
				dynamicObjsCollection.erase(it);
				break;
			}
		}
	}
	delete markedForDeletion;
}

//
//	FUNCTION: fitCoordsInMap(POINT*)
//
//	PURPOSE:
//		Converts coordinates that lay beyond the map to coordinates within the map with modulo operation
//
//	PARAMS:
//		POINT* ptCoords - pointer to POINT struct that specifies coordinates to convert and retrieves result of operation
//
void SnakeMap::fitCoordsInMap(POINT *ptCoords) {
	if ((ptCoords->x %= mapWidth) < 0)
		ptCoords->x += mapWidth;
	if ((ptCoords->y %= mapHeight) < 0)
		ptCoords->y += mapHeight;
}

//
//	FUNCTION: snakeMovedHandler(Snake*, int, int, int, int)
//
//	PURPOSE:
//		Handles Snake::SnakeMoved event; updates snake's position on map overlay table
//
//	PARAMS: see Snake::SnakeMoved event
//
void SnakeMap::snakeMovedHandler(Snake *snake, int headX, int headY, int lastTailX, int lastTailY) {
	if (snake != this->snake)
		return;

	POINT ptHead = { headX, headY };
	POINT ptTail = { lastTailX, lastTailY };
	fitCoordsInMap(&ptHead);
	fitCoordsInMap(&ptTail);
	snake->setPosition(ptHead.x, ptHead.y);

	mapOverlay[getMapOverlayIndex(ptTail.x, ptTail.y)] = nullptr;

	ObjectOnMap *checkCollision = getObject(ptHead.x, ptHead.y);
	if (checkCollision)
		checkCollision->raiseSnakeCollisionEvent(snake);
	
	mapOverlay[getMapOverlayIndex(ptHead.x, ptHead.y)] = snake;
}

//
//	FUNCTION: objectPosChangedHandler(ObjectOnMap*, int, int)
//
//	PURPOSE:
//		Handles ObjectOnMap::PositionChanged event; updates object's position on map overlay and remediate its position to fit on map;
//			should not be used for objects that don't fit in one tile e.g. Snake object
//
//	PARAMS: see ObjectOnMap::PositionChanged
//
void SnakeMap::objectPosChangedHandler(DynamicObjectBase *obj, int prevX, int prevY) {
	POINT ptPos = { obj->getPosX(), obj->getPosY() };
	fitCoordsInMap(&ptPos);
	if (ptPos.x != obj->getPosX() || ptPos.y != obj->getPosY())
		obj->setPositionWithoutEvent(ptPos.x, ptPos.y);

	ObjectOnMap *checkCollision = getObject(ptPos.x, ptPos.y);
	if (checkCollision) {
		if (!obj->onCollision(checkCollision)) {
			obj->setPositionWithoutEvent(prevX, prevY);
			return;
		}
	}

	mapOverlay[getMapOverlayIndex(prevX, prevY)] = nullptr;
	mapOverlay[getMapOverlayIndex(obj->getPosX(), obj->getPosY())] = obj;
}

void SnakeMap::snakeNewElementAddedHandler(Snake *snake) {
	if (snake != this->snake)
		return;

	POINT ptNewTail;
	snake->getTailPosition(&ptNewTail);
	fitCoordsInMap(&ptNewTail);
	if (getObject(ptNewTail.x, ptNewTail.y))
		throw SnakeElementAdditionException();
	mapOverlay[getMapOverlayIndex(ptNewTail.x, ptNewTail.y)] = snake;
}