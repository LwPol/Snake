#include "stdafx.h"
#include "snake.h"
#include "snakeGameExceptions.h"

//
//	CONSTRUCTOR: Snake(int, int, ObjectOnMap::Orientation)
//
//	COMMENT: delegates DynamicObjectBase(int, int)
//
//	PARAMS:
//		int x - same as in DynamicObjectBase ctor
//		int y - same as in DynamicObjectBase ctor
//		Orientation headOrientation - orientation of the head
//
Snake::Snake(int x, int y, Orientation headOrientation) :
	DynamicObject(x, y)
{
	auto bitmapLoadFail = [](const std::string &sPath, int lastError) {
		if (lastError == ERROR_FILE_NOT_FOUND || lastError == ERROR_PATH_NOT_FOUND)
			throw BitmapLoadFail("Nie znaleziono pliku " + sPath);
		else
			throw BitmapLoadFail("Wyst¹pi³ b³¹d podczas ³adowania bitmapy " + sPath, lastError);
	};

	// loading snake's bitmaps and masks
	std::string sFiles[] = { "head", "body", "turn", "tail" };
	std::string sDirectory = "graphics\\", sBmp = ".bmp";
	SetLastError(0);
	for (int i = 0; i < static_cast<int>(BodyPart::COUNT); ++i) {
		hbmSnakeParts[i][0] = reinterpret_cast<HBITMAP>(LoadImageA(nullptr, (sDirectory + sFiles[i] + sBmp).c_str(),
			IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE));
		if (!hbmSnakeParts[i][0]) {
			int error = GetLastError();
			bitmapLoadFail(sDirectory + sFiles[i] + sBmp, error);
		}

		for (int j = 1; j < static_cast<int>(Orientation::COUNT); ++j) {
			hbmSnakeParts[i][j] = createRotatedBitmap(hbmSnakeParts[i][0], static_cast<RotationType>(j - 1));
			hbmSnakePartsMasks[i][j] = createBitmapMask(hbmSnakeParts[i][j], RGB(255, 0, 255));

			if (!hbmSnakeParts[i][j] || !hbmSnakePartsMasks[i][j]) {
				int error = GetLastError();
				bitmapLoadFail("", error);
			}
		}
		hbmSnakePartsMasks[i][0] = createBitmapMask(hbmSnakeParts[i][0], RGB(255, 0, 255));
		if (!hbmSnakePartsMasks[i][0]) {
			int error = GetLastError();
			bitmapLoadFail("", error);
		}
	}
	// set bitmaps' size
	BITMAP bmInfo;
	GetObject(hbmSnakeParts[0][0], sizeof(BITMAP), &bmInfo);
	bitmapWidth = bmInfo.bmWidth;
	bitmapHeight = bmInfo.bmHeight;

	// create snake parts
	snake.push_back({ BodyPart::HEAD, headOrientation, getOppositeDirection(headOrientation) });
	snake.push_back({ BodyPart::BODY, headOrientation, getOppositeDirection(headOrientation) });
	snake.push_back({ BodyPart::TAIL, headOrientation, getOppositeDirection(headOrientation) });
}

//
//	FUNCTION: draw(HDC, int, int, int, int)
//
//	PURPOSE:
//		Draws snake in given device taking into account map size
//
//	PARAMS:
//		HDC hdc - handle to DC to draw into
//		int fieldWidth - width of a single tile on the map
//		int fieldHeight - height of a single tile on the map
//		int mapWidth - width of the map, in tiles
//		int mapHeight - height of the map, in tiles
//
void Snake::draw(HDC hdc, int fieldWidth, int fieldHeight, int mapWidth, int mapHeight) {
	// set stretch blt mode
	int stretchMode = GetStretchBltMode(hdc);
	SetStretchBltMode(hdc, HALFTONE);

	// drawing loop
	SmartHandle<HDC> hdcMem(CreateCompatibleDC(nullptr));
	POINT ptPosition = { getPosX(), getPosY() };
	getNextLocationFromOrientation(snake.front().nextPartLocation, &ptPosition);
	for (std::size_t i = 1; i < snake.size() + 1; ++i) {
		if (i == snake.size())
			ptPosition = { getPosX(), getPosY() };
		if ((ptPosition.x %= mapWidth) < 0)
			ptPosition.x += mapWidth;
		if ((ptPosition.y %= mapHeight) < 0)
			ptPosition.y += mapHeight;

		// snake's head is a last drawn element
		SnakeElement *cur;
		if (i < snake.size())
			cur = &snake[i];
		else
			cur = &snake.front();

		// draw current element
		hdcMem.setBitmap(hbmSnakePartsMasks[static_cast<int>(cur->part)][static_cast<int>(cur->bodyPartOrientation)]);
		StretchBlt(hdc,
			ptPosition.x * fieldWidth, ptPosition.y * fieldHeight,
			fieldWidth, fieldHeight,
			hdcMem,
			0, 0, bitmapWidth, bitmapHeight, SRCAND);
		hdcMem.setBitmap(hbmSnakePartsMasks[static_cast<int>(cur->part)][static_cast<int>(cur->bodyPartOrientation)]);

		hdcMem.setBitmap(hbmSnakeParts[static_cast<int>(cur->part)][static_cast<int>(cur->bodyPartOrientation)]);
		StretchBlt(hdc,
			ptPosition.x * fieldWidth, ptPosition.y * fieldHeight,
			fieldWidth, fieldHeight,
			hdcMem,
			0, 0, bitmapWidth, bitmapHeight, SRCPAINT);
		hdcMem.setBitmap(hbmSnakeParts[static_cast<int>(cur->part)][static_cast<int>(cur->bodyPartOrientation)]);

		// set position for drawing next element
		getNextLocationFromOrientation(cur->nextPartLocation, &ptPosition);
	}

	// prepare for return
	SetStretchBltMode(hdc, stretchMode);
}

//
//	FUNCTION: draw(HDC, int, int)
//
//	COMMENT: overrides ObjectOnMap::draw(HDC, int, int); to correctly display snake on the map function draw(HDC, int, int, int, int)
//		should be used instead
//
void Snake::draw(HDC hdc, int fieldWidth, int fieldHeight) {
	draw(hdc, fieldWidth, fieldHeight,
		std::numeric_limits<int>::max(), std::numeric_limits<int>::max());
}

//
//	FUNCTION: update(int)
//
//	COMMENT: overrides DynamicObjectBase::update(int)
//
void Snake::update(int timePassed) {
	DynamicObjectBase::update(timePassed);
	// update snake's state
	if (time >= oneTileTime) {
		time -= oneTileTime;

		// move snake
		bool bValidCommandProcessed = false;
		while (!bValidCommandProcessed && !moveCommands.empty()) {
			bValidCommandProcessed = processMoveCommand(moveCommands.front());
			moveCommands.pop();
		}
		// if no valid commands were encountered, just keep moving in head's direction
		if (!bValidCommandProcessed)
			processMoveCommand(snake.front().bodyPartOrientation);
	}
}

//
//	FUNCTION: addMoveCommand(ObjectOnMap::Orientation)
//
//	PURPOSE:
//		Adds new move command to snake's queue of commands
//
//	PARAMS:
//		Orientation moveCmd - command to add to the queue; if command is the same as last command in the queue, this function has no effects
//
void Snake::addMoveCommand(Orientation moveCmd) {
	if (moveCommands.empty() || moveCommands.back() != moveCmd)
		moveCommands.push(moveCmd);
}

//
//	FUNCTION: addElement()
//
//	PURPOSE:
//		Adds a new element at the end of snake tail
//
void Snake::addElement() {
	// get reference to current tail
	SnakeElement &currentTail = snake.back();
	// create new tail
	SnakeElement newTail;
	newTail.part = BodyPart::TAIL;
	newTail.bodyPartOrientation = getOppositeDirection(currentTail.nextPartLocation);
	newTail.nextPartLocation = currentTail.nextPartLocation;
	snake.push_back(newTail);

	// change previous tail to match the snake
	currentTail = getMatchingPart(currentTail.bodyPartOrientation, currentTail.nextPartLocation);

	// fire event
	NewElementAdded(this);
}

//
//	FUNCTION: getTailPosition(POINT*)
//
//	PURPOSE:
//		Retrieves tail's position
//
//	PARAMS:
//		POINT* ptTailPosition - retrieves position of the tail
//
void Snake::getTailPosition(POINT *ptTailPosition) const {
	*ptTailPosition = { getPosX(), getPosY() };
	for (auto it = snake.begin(); it->part != BodyPart::TAIL; ++it)
		getNextLocationFromOrientation(it->nextPartLocation, ptTailPosition);
}

//
//	FUNCTION: getSnakeLength()
//
//	PURPOSE:
//		Gets size of the snake, in number of parts it consists of
//
//	RETURNS: int
//		Returns snake's length
//
int Snake::getSnakeLength() const {
	return snake.size();
}

//
//	FUNCTION: getMatchingPart(ObjectOnMap::Orientation, ObjectOnMap::Orientation)
//
//	PURPOSE:
//		Gets a snake part that matches two directions
//
//	PARAMS:
//		Orientation in - direction to preceeding element (elements are counted from head)
//		Orientation out - direction to proceeding element
//
//	RETURNS: Snake::SnakeElement
//		Returns a snake part that matches given directions; type of the element is always BodyPart::BODY or BodyPart::TURN
//
Snake::SnakeElement Snake::getMatchingPart(Orientation in, Orientation out) {
	if (in < Orientation::UP || in >= Orientation::COUNT || out < Orientation::UP || out >= Orientation::COUNT)
		throw std::invalid_argument("Snake::getMatchingPart: Invalid enum value");
	if (in == out)
		throw std::invalid_argument("Snake::getMatchingPart:"
			"in and out cannot be the same");

	SnakeElement matchingPart;
	if (in == getOppositeDirection(out)) {
		matchingPart.part = BodyPart::BODY;
		matchingPart.bodyPartOrientation = in;
	}
	else {
		matchingPart.part = BodyPart::TURN;
		if (in == getNextDirection(out, true))
			matchingPart.bodyPartOrientation = getOppositeDirection(out);
		else
			matchingPart.bodyPartOrientation = getOppositeDirection(in);
	}
	matchingPart.nextPartLocation = out;

	return matchingPart;
}

//
//	FUNCTION: processMoveCommand(ObjectOnMap::Orientation)
//
//	PURPOSE:
//		Processes a move command and updates snake position accordingly
//
//	PARAMS:
//		Orientation moveCmd - direction in which snake should move
//
//	RETURNS: bool
//		Returns true if move is possible and snake's position has been updated, and false otherwise
//
bool Snake::processMoveCommand(Orientation moveCmd) {
	// snake can't make 180 turn
	if (getOppositeDirection(snake.front().bodyPartOrientation) == moveCmd)
		return false;

	// retrieve current tail's position
	POINT ptLastTailPos;
	getTailPosition(&ptLastTailPos);

	SnakeElement sePrev = snake.front();
	// move snake's head
	POINT ptHead = { getPosX(), getPosY() };
	getNextLocationFromOrientation(moveCmd, &ptHead);
	setPosition(ptHead.x, ptHead.y);
	snake.front().bodyPartOrientation = moveCmd;
	snake.front().nextPartLocation = getOppositeDirection(moveCmd);

	// process remaining parts of the snake
	for (std::size_t i = 1; i < snake.size(); ++i) {
		SnakeElement &seCurrent = snake[i];
		SnakeElement seCurrentCopy = seCurrent;
		if (seCurrent.part == BodyPart::TAIL) {
			if (sePrev.part == BodyPart::TURN) {
				if (getOppositeDirection(sePrev.bodyPartOrientation) == sePrev.nextPartLocation)
					seCurrent.bodyPartOrientation = getNextDirection(sePrev.nextPartLocation, true);
				else
					seCurrent.bodyPartOrientation = getOppositeDirection(sePrev.bodyPartOrientation);
			}
			else if (sePrev.part == BodyPart::HEAD)
				seCurrent.bodyPartOrientation = sePrev.bodyPartOrientation;
			seCurrent.nextPartLocation = sePrev.nextPartLocation;
		}
		else {
			if (sePrev.part == BodyPart::HEAD)
				seCurrent = getMatchingPart(moveCmd, sePrev.nextPartLocation);
			else
				seCurrent = sePrev;
		}
		sePrev = seCurrentCopy;
	}

	// fire event
	SnakeMoved(this, ptHead.x, ptHead.y, ptLastTailPos.x, ptLastTailPos.y);
	return true;
}