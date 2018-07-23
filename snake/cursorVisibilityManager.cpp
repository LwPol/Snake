#include "stdafx.h"
#include "cursorVisibilityManager.h"

CursorVisibilityManager::CursorVisibilityManager(bool bVisible) :
	bVisible(bVisible)
{
	int displayCount;
	if (bVisible) {
		while ((displayCount = ShowCursor(SW_SHOW)) < 0) {}
		if (displayCount > 0)
			while (ShowCursor(SW_HIDE) > 0) {}
	}
	else {
		while ((displayCount = ShowCursor(SW_HIDE)) >= 0) {}
		if (displayCount < -1)
			while (ShowCursor(SW_SHOW) < -1) {}
	}
}

void CursorVisibilityManager::showCursor() {
	if (!bVisible) {
		ShowCursor(SW_SHOW);
		bVisible = true;
	}
}

void CursorVisibilityManager::hideCursor() {
	if (bVisible) {
		ShowCursor(SW_HIDE);
		bVisible = false;
	}
}
