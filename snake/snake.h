#pragma once

#include "dynamicObject.h"

class Snake : public DynamicObject<Snake> {
	public:
		/// types

		//	ENUM: BodyPart
		//	Defines particular part of snake body
		enum class BodyPart {
			HEAD, BODY, TURN, TAIL, COUNT
		};
		
		//	C-STRUCT: SnakeElement
		//	Data for a single element of snake body oriented on map
		struct SnakeElement {
			BodyPart part; // particular body part
			Orientation bodyPartOrientation; // orientation of this part
			Orientation nextPartLocation; // where to look for next part
		};

	private:
		/// private fields

		// number of miliseconds to pass to update position
		int oneTileTime = 150;

		// queue of user's move commands
		std::queue<Orientation> moveCommands;

		// collection creating snake body
		std::deque<SnakeElement> snake;

		// bitmaps for snake's body parts differently orientated
		SmartHandle<HBITMAP> hbmSnakeParts[(int)BodyPart::COUNT][(int)Orientation::COUNT];
		// masks for those bitmaps
		SmartHandle<HBITMAP> hbmSnakePartsMasks[(int)BodyPart::COUNT][(int)Orientation::COUNT];
		// bitmaps size (all should have the same size)
		int bitmapWidth, bitmapHeight;

		/// private methods
		bool processMoveCommand(Orientation moveCmd);
	public:
		/// ctors
		Snake(int x, int y, Orientation headOrientation);

		/// public methods
		void draw(HDC hdc, int fieldWidth, int fieldHeight, int mapWidth, int mapHeight);
		void draw(HDC hdc, int fieldWidth, int fieldHeight) override;

		void update(int timePassed) override;
		void addMoveCommand(Orientation moveCmd);

		void addElement();

		void getTailPosition(POINT *ptTailCoords) const;
		int getSnakeLength() const;

		static SnakeElement getMatchingPart(Orientation in, Orientation out);

		/// accessors
		int getOneTileTime() const { return oneTileTime; }
		void setOneTileTime(int val) {
			if (val > 0)
				oneTileTime = val;
		}
		Orientation getHeadOrientation() const { return snake.front().bodyPartOrientation; }

		/// events

		//	EVENT: SnakeMoved
		//	Notifies when snake moved. Sent after snake's position has been changed
		//
		//	PARAMS:
		//		Snake* - object sending notification
		//		int - x coordinate of snake's head new position
		//		int - y coordinate of snake's head new position
		//		int - x coordinate of snake's tail previous position
		//		int - y coordinate of snake's tail previous position
		boost::signals2::signal<void(Snake*, int, int, int, int)> SnakeMoved;

		/// doc!
		boost::signals2::signal<void(Snake*)> NewElementAdded;
};