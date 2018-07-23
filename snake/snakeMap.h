#pragma once

#include "dynamicObjectBase.h"
#include "staticObject.h"
#include "snake.h"

class SnakeMap {
	friend class SnakeMapLoader;
	private:
		/// private fields

		// map's size
		int mapWidth, mapHeight;
		// a single tile's size
		static constexpr int fieldWidth = 30, fieldHeight = 30;

		// collection of static objects on map
		std::list<StaticObject*> staticObjsCollection;
		// collection of dynamic objects on map (except for snake)
		std::list<DynamicObjectBase*> dynamicObjsCollection;

		// map layout - NULL pointer in table means no object on that position
		std::vector<ObjectOnMap*> mapOverlay;

		// snake object
		Snake *snake;

		// brush to draw background with
		SmartHandle<HBRUSH> hbrBackground;

		/// private methods

		//
		//	FUNCTION: getMapOverlayIndex(int, int)
		//
		//	PURPOSE:
		//		Gets index in mapOverlay table coresponding to given coordinates on the map
		//
		//	PARAMS:
		//		int x - x coordinate on map; should be in range [0, mapWidth)
		//		int y - y coordinate on map; should be in range [0, mapHeight)
		//
		//	RETURNS: int
		//		Returns index corresponding to (x, y) point on the map in mapOverlay table
		//
		int getMapOverlayIndex(int x, int y) { return y * mapWidth + x; }
		void fitCoordsInMap(POINT *ptCoords);
		void snakeMovedHandler(Snake *snake, int headX, int headY, int lastTailX, int lastTailY);
		void objectPosChangedHandler(DynamicObjectBase *obj, int prevX, int prevY);
		void snakeNewElementAddedHandler(Snake *snake);

		template <class T>
		void addObjectToCollection(T *obj, ObjectOnMap::DynamicObjectTag tag) {
			dynamicObjsCollection.push_back(obj);
			obj->PositionChanged.connect(boost::bind(&SnakeMap::objectPosChangedHandler, this, _1, _2, _3));
		}
		template <class T>
		void addObjectToCollection(T *obj, ObjectOnMap::StaticObjectTag tag) {
			staticObjsCollection.push_back(obj);
		}
		template <class T>
		void addObjectToCollection(T *obj) {
			addObjectToCollection(obj, typename T::object_on_map_category());
		}
	public:
		/// ctors
		SnakeMap(int width, int height);
		SnakeMap(const std::string &sFilePath);
		SnakeMap(const SnakeMap&) = delete;
		SnakeMap(SnakeMap &&other);

		SnakeMap& operator=(const SnakeMap&) = delete;
		SnakeMap& operator=(SnakeMap &&other);

		/// dtor
		~SnakeMap();

		/// public methods
		void drawMap(HDC hdc);

		void update(int timePassed);

		ObjectOnMap* getObject(int x, int y);
		template <class T, class ...Args>
		bool addObject(int x, int y, Args&& ...args);
		bool createSnake(int x, int y, ObjectOnMap::Orientation headOrientation);

		void deleteObject(DynamicObjectBase *obj);
		void deleteObject(int x, int y);

		/// accessors
		int getWidth() const { return mapWidth; }
		int getHeight() const { return mapHeight; }
		int getWidthPixels() const { return mapWidth * fieldWidth; }
		int getHeightPixels() const { return mapHeight * fieldHeight; }
		static constexpr int getFieldWidth() { return fieldWidth; }
		static constexpr int getFieldHeight() { return fieldHeight; }

		Snake* getSnake() { return snake; }
		const Snake* getSnake() const { return snake; }
};

//
//	FUNCTION TEMPLATE: addObject<T, ...Args>(int, int, Args&&...)
//
//	PURPOSE:
//		Adds new object of type T to the map in given location
//
//	TEMPLATE PARAMS:
//		typename T - type of object to add; this type must inherit from ObjectOnMap class, it cannot be Snake class and
//			must have ctor with signature (int, int, ...)
//		typename ...Args - additional arguments
//
//	PARAMS:
//		int x - x coordinate of object's position
//		int y - y coordinate of object's position
//		Args&&... args - arguments for object's ctor proceeding object's coordinates
//
//	RETURNS: bool
//		Returns true if object was successfully created, and false otherwise; functions fails if an existing object is already
//			on the map in specified location or coordinates lay beyond the map
//
template <class T, class ...Args>
bool SnakeMap::addObject(int x, int y, Args&& ...args) {
	static_assert(std::is_base_of<ObjectOnMap, T>::value, "Invalid type for SnakeMap::addObject template method. "
		"Types used with this template must inherit from ObjectOnMap class.");
	static_assert(!std::is_same<T, Snake>::value, "Snake class is not allowed as template parameter of SnakeMap::addObject method. "
		"Use SnakeMap::createSnake method instead.");
	static_assert(std::is_constructible<T, int, int, Args...>::value, "Cannot construct object of this type with given parameters");

	if (x < 0 || x >= mapWidth || y < 0 || y >= mapHeight)
		throw std::invalid_argument("SnakeMap::addObject: Coordinates lay beyond current map size");

	if (getObject(x, y))
		return false;

	T *newObject = new T(x, y, std::forward<Args>(args)...);
	addObjectToCollection(newObject);
	mapOverlay[getMapOverlayIndex(x, y)] = newObject;
	return true;
}