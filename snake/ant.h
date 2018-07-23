#pragma once

#include "dynamicObject.h"

class Ant : public DynamicObject<Ant> {
	private:
		static SmartHandle<HBITMAP> hbmAnt[static_cast<int>(Orientation::COUNT)];
		static SmartHandle<HBITMAP> hbmAntMask[static_cast<int>(Orientation::COUNT)];

		int oneTileTime = 150;

		Orientation movingDirection;
	public:
		Ant(int x, int y, Orientation move);
		Ant(int x, int y, Orientation move, const boost::function<void(Ant*)> &disposedHandler);

		~Ant();

		void draw(HDC hdc, int fieldWidth, int fieldHeight) override;

		void update(int timePassed) override;

		bool onCollision(ObjectOnMap *other) override;

		static bool loadAntBitmaps(const std::string &baseBitmapPath);
		static void unloadAntBitmaps();
};