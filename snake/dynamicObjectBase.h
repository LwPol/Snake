#pragma once

#include "objectOnMap.h"

class DynamicObjectBase : public ObjectOnMap {
	public:
		typedef DynamicObjectTag object_on_map_category;
	protected:
		int time = 0;
	public:
		/// ctors

		//
		//	CONSTRUCTOR: DynamicObjectBase(int, int)
		//
		//	COMMENT: delegates to ObjectOnMap(int, int) ctor
		//
		DynamicObjectBase(int x, int y) : ObjectOnMap(x, y) {}

		/// dtor
		virtual ~DynamicObjectBase() = default;

		/// public methods
		virtual void detachDisposeHandlers() = 0;

		//
		//	FUNCTION: update(int)
		//
		//	PURPOSE:
		//		updates state of the object
		//
		//	PARAMS:
		//		int timePassed - time passed since last update
		//
		virtual void update(int timePassed) {
			time += timePassed;
		}

		virtual bool onCollision(ObjectOnMap *other) { return false; }

		void setPosition(int x, int y) override {
			int copyX = getPosX(), copyY = getPosY();
			ObjectOnMap::setPosition(x, y);
			PositionChanged(this, copyX, copyY);
		}
		void setPositionWithoutEvent(int x, int y) {
			ObjectOnMap::setPosition(x, y);
		}

	public:
		boost::signals2::signal<void(DynamicObjectBase*, int, int)> PositionChanged;
};