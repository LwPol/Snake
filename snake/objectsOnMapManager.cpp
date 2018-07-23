#include "stdafx.h"
#include "objectsOnMapManager.h"

void ObjectsOnMapManager::update(int timePassed) {
	ObjectsOnMapManagerBase::update(timePassed);
	updateAntState(timePassed);
}

void ObjectsOnMapManager::updateAntState(int timePassed) {
	if (numOfAnts == 0) {
		timeForAntCreation += timePassed;
		if (timeForAntCreation >= timeSliceForAntCreation) {
			timeForAntCreation = 0;

			std::uniform_int_distribution<> dist(1, 10);
			std::uniform_int_distribution<> orientation(0, static_cast<int>(ObjectOnMap::Orientation::COUNT) - 1);
			std::uniform_int_distribution<> xCoord(0, getManagedMap()->getWidth() - 1);
			std::uniform_int_distribution<> yCoord(0, getManagedMap()->getHeight() - 1);
			int numOfAntsToCreate = dist(randomNumGenerator) == 1 ? 2 : 1;
			ObjectOnMap::Orientation movingDir = static_cast<ObjectOnMap::Orientation>(orientation(randomNumGenerator));
			for (int i = 0; i < numOfAntsToCreate; ++i) {
				while (!getManagedMap()->addObject<Ant>(xCoord(randomNumGenerator), yCoord(randomNumGenerator),
					movingDir, boost::bind(&ObjectsOnMapManager::objectDeletionHandler<Ant>, this, _1))) {}
			}
			numOfAnts = numOfAntsToCreate;
		}
	}
}
