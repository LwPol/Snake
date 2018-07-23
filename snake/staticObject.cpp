#include "stdafx.h"
#include "staticObject.h"

boost::signals2::signal<void(StaticObject*, Snake*)> StaticObject::SnakeCollision;