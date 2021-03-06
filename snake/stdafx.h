// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#define _SCL_SECURE_NO_WARNINGS

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <windowsx.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#undef max
#undef min

// TODO: reference additional headers your program requires here
#include <functional>
#include <vector>
#include <list>
#include <string>
#include <fstream>
#include <cassert>
#include <memory>
#include <deque>
#include <queue>
#include <set>
#include <exception>
#include <limits>
#include <type_traits>
#include <random>
#include <chrono>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <boost\signals2\signal.hpp>

#include "application.h"
#include "windowProc.h"
#include "smartHandle.h"
#include "controlBase.h"

#pragma warning(disable : 4244)
#pragma warning(disable : 4018)

namespace SnakeGameConstants
{
	constexpr int mapMaxWidth = 40, mapMaxHeight = 20;
	constexpr int mapMinWidth = 30, mapMinHeight = 15;
}