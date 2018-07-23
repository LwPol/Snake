#pragma once

#include "stdafx.h"

class CursorVisibilityManager {
	private:
		bool bVisible;
	public:
		CursorVisibilityManager(bool bVisible);

		void showCursor();
		void hideCursor();

		bool isCursorVisible() const { return bVisible; }
};