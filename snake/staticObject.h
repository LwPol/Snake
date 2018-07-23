#pragma once

#include "objectOnMap.h"

class StaticObject : public ObjectOnMap {
	public:
		typedef StaticObjectTag object_on_map_category;

		StaticObject(int x, int y) :
			ObjectOnMap(x, y)
		{
		}

		void raiseSnakeCollisionEvent(Snake *snake) override final {
			SnakeCollision(this, snake);
		}
		
		static boost::signals2::signal<void(StaticObject*, Snake*)> SnakeCollision;
};