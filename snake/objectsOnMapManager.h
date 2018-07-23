#pragma once

#include "objectsOnMapManagerBase.h"
#include "basicFood.h"
#include "ant.h"

class ObjectsOnMapManager : public ObjectsOnMapManagerBase {
	private:
		std::mt19937 randomNumGenerator;

		int numOfAnts = 0;
		int timeForAntCreation = 0;
		int timeSliceForAntCreation = 3000;

		void updateAntState(int timePassed);
	public:
		ObjectsOnMapManager(SnakeMap *map = nullptr) :
			ObjectsOnMapManagerBase(map)
		{
			unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
			randomNumGenerator.seed(seed);
		}

		virtual void update(int timePassed) override;

		template <class ManagedType>
		void objectDeletionHandler(ManagedType *obj);
		template <>
		void objectDeletionHandler(BasicFood *obj);
		template <>
		void objectDeletionHandler(Ant *obj);
};

template <class ManagedType>
void ObjectsOnMapManager::objectDeletionHandler(ManagedType *obj) {
	static_assert(std::is_base_of<DynamicObjectBase, ManagedType>::value, "ObjectOnMapManager can only manage dynamic objects");
}

template <>
void ObjectsOnMapManager::objectDeletionHandler(BasicFood *obj) {
	Snake *snake = getManagedMap()->getSnake();

	std::uniform_int_distribution<> xCoordDist(0, getManagedMap()->getWidth() - 1);
	std::uniform_int_distribution<> yCoordDist(0, getManagedMap()->getHeight() - 1);

	bool bSucceed = false;
	while (!bSucceed) {
		int x = xCoordDist(randomNumGenerator);
		int y = yCoordDist(randomNumGenerator);
		if (snake && x == snake->getPosX() && y == snake->getPosY())
			continue;
		bSucceed = getManagedMap()->addObject<BasicFood>(x, y,
			boost::bind(&ObjectsOnMapManager::objectDeletionHandler<BasicFood>, this, _1));
	}
}

template <>
void ObjectsOnMapManager::objectDeletionHandler(Ant *obj) {
	--numOfAnts;
}