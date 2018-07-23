#pragma once

#include "dynamicObjectBase.h"

template <class ObjType>
class DynamicObject : public DynamicObjectBase {
	private:
		void detachDisposeHandlers() override {
			Disposed.disconnect_all_slots();
		}
	public:
		DynamicObject(int x, int y) :
			DynamicObjectBase(x, y)
		{
		}

		DynamicObject(int x, int y, const boost::function<void(ObjType*)> &disposedHandler) :
			DynamicObjectBase(x, y)
		{
			addDisposedHandler(disposedHandler);
		}

		virtual ~DynamicObject() = default;

		void addDisposedHandler(const boost::function<void(ObjType*)> &disposedHandler) {
			Disposed.connect(disposedHandler);
		}

		void raiseSnakeCollisionEvent(Snake *snake) override final {
			SnakeCollision(static_cast<ObjType*>(this), snake);
		}
	protected:
		boost::signals2::signal<void(ObjType*)> Disposed;
	public:
		static boost::signals2::signal<void(ObjType*, Snake*)> SnakeCollision;
};

template <class ObjType>
boost::signals2::signal<void(ObjType*, Snake*)> DynamicObject<ObjType>::SnakeCollision;