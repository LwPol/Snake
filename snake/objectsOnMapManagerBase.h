#pragma once

#include "stdafx.h"
#include "snakeMap.h"

class ObjectsOnMapManagerBase {
	private:
		SnakeMap *managedMap;
	protected:
		int time = 0;

		ObjectsOnMapManagerBase(SnakeMap *map = nullptr) :
			managedMap(map) {}
	public:
		virtual ~ObjectsOnMapManagerBase() = default;

		virtual void update(int timePassed) {
			time += timePassed;
		}

		SnakeMap* getManagedMap() { return managedMap; }
		void setManagedMap(SnakeMap *map) { managedMap = map; }
};
