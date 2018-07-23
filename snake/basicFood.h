#pragma once

#include "dynamicObject.h"

class BasicFood : public DynamicObject<BasicFood> {
	private:
		/// private fields

		// food bitmap and its mask
		static SmartHandle<HBITMAP> hbmFood;
		static SmartHandle<HBITMAP> hbmFoodMask;
	public:
		/// ctors
		template <class ...Args>
		BasicFood(Args&& ...args) :
			DynamicObject(std::forward<Args>(args)...) {}

		/// dtor
		~BasicFood();

		/// public methods
		static bool loadBitmaps(const std::string &bitmapPath);
		static void unloadBitmaps();

		void draw(HDC hdc, int fieldWidth, int fieldHeight) override;
};