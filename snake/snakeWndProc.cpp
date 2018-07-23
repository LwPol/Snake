#include "stdafx.h"
#include "snakeWndProc.h"
#include "startingMenu.h"
#include "snakeGameExceptions.h"

SnakeWndProc::SnakeWndProc() :
	smMap("maps\\box.sm"),
	oommManager(&smMap),
	sdDisp(smMap.getWidthPixels(), 40),
	cvmCursor(true)
{
	hdcMem = CreateCompatibleDC(nullptr);
	if (!hdcMem)
		throw GdiObjectCreationException();
	SmartHandle<HBITMAP> hbmMem = CreateBitmap(smMap.getWidthPixels(), smMap.getHeightPixels(), 1, 32, nullptr);
	if (!hbmMem)
		throw GdiObjectCreationException();
	hdcMem.setBitmap(std::move(hbmMem));

	std::mt19937 generator;
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	generator.seed(seed);
	smMap.createSnake(
		std::uniform_int_distribution<>(3, smMap.getWidth() - 4)(generator),
		std::uniform_int_distribution<>(3, smMap.getHeight() - 4)(generator),
		(ObjectOnMap::Orientation)std::uniform_int_distribution<>(0, (int)ObjectOnMap::Orientation::COUNT - 1)(generator)
	);
	smMap.getSnake()->NewElementAdded.connect(boost::bind(&SnakeWndProc::snakeNewElementAddedHandler, this, _1));
	std::uniform_int_distribution<> firstFoodX(0, smMap.getWidth() - 1), firstFoodY(0, smMap.getHeight() - 1);
	auto basicFoodDeletionHandler = boost::bind(&ObjectsOnMapManager::objectDeletionHandler<BasicFood>, &oommManager, _1);
	while (!smMap.addObject<BasicFood>(firstFoodX(generator), firstFoodY(generator), basicFoodDeletionHandler)) {}

	if (Application::hWnd())
		initializeGame(Application::hWnd());
}

SnakeWndProc::~SnakeWndProc() {
	cvmCursor.showCursor();
	detachCollisionHandlers();
	KillTimer(Application::hWnd(), 0);
	BasicFood::unloadBitmaps();
	Ant::unloadAntBitmaps();
}

LRESULT SnakeWndProc::wndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
		case WM_CREATE:
			initializeGame(hWnd);
			break;
		case WM_KEYDOWN:
			if (!(lParam & (1 << 30))) {
				switch (wParam) {
					case VK_UP:
						smMap.getSnake()->addMoveCommand(Snake::Orientation::UP);
						break;
					case VK_LEFT:
						smMap.getSnake()->addMoveCommand(Snake::Orientation::LEFT);
						break;
					case VK_DOWN:
						smMap.getSnake()->addMoveCommand(Snake::Orientation::DOWN);
						break;
					case VK_RIGHT:
						smMap.getSnake()->addMoveCommand(Snake::Orientation::RIGHT);
						break;
					case L'A':
						if (smMap.getSnake()->getOneTileTime() == 150)
							smMap.getSnake()->setOneTileTime(70);
						else
							smMap.getSnake()->setOneTileTime(150);
						break;
					case VK_ESCAPE:
						Application::setNewWindowProc<StartingMenu>();
						break;
				}
				if (!bTimersStarted) {
					if (wParam == VK_UP || wParam == VK_LEFT || wParam == VK_DOWN || wParam == VK_RIGHT) {
						startTimer(hWnd);
						sdDisp.startTimer();
						bTimersStarted = true;
					}
				}
			}
			break;
		case WM_ACTIVATE:
			if (wParam != WA_INACTIVE)
				if (gswSummary.window())
					SetFocus(gswSummary.window());
			break;
		case WM_TIMER:
			switch (wParam) {
				case 0:
					timerProc(GetTickCount());
					break;
			}
			break;
		case WM_MOUSEMOVE:
			cvmCursor.hideCursor();
			break;
		case WM_NCMOUSEMOVE:
			cvmCursor.showCursor();
			break;
		case WM_PAINT: {
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);
			smMap.drawMap(hdcMem);
			sdDisp.render(hdc, 0, 0);
			BitBlt(hdc, 0, sdDisp.getHeight(), smMap.getWidthPixels(), smMap.getHeightPixels(),
				hdcMem, 0, 0, SRCCOPY);
			EndPaint(hWnd, &ps);
			break;
		}
		case WM_DESTROY:
			Application::setNewWindowProc<void>();
			break;
		default:
			return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

void SnakeWndProc::startTimer(HWND hWnd) {
	dwLastTick = GetTickCount();
	if (!SetTimer(hWnd, 0, 30, nullptr)) {
		throw std::exception("Timer failed!");
	}
}

void SnakeWndProc::endGame() {
	if (gswSummary.window())
		return;

	KillTimer(Application::hWnd(), 0);
	sdDisp.stopTimer();

	RECT rcMainWnd;
	GetClientRect(Application::hWnd(), &rcMainWnd);

	gswSummary = GameSummaryWindow(sdDisp.getScore(), sdDisp.getSecondsInGame());
	gswSummary.create(nullptr,
		(rcMainWnd.right - GameSummaryWindow::width) / 2, (rcMainWnd.bottom - GameSummaryWindow::height) / 2,
		0, 0, Application::hWnd());
}

void SnakeWndProc::timerProc(DWORD dwTime) {
	DWORD dwTimePassed = dwTime - dwLastTick;
	dwLastTick = dwTime;

	oommManager.update(dwTimePassed);
	smMap.update(dwTimePassed);
	RedrawWindow(Application::hWnd(), nullptr, nullptr, RDW_INVALIDATE);
}

void SnakeWndProc::initCollisionHandlers() {
	attachSnakeCollisionHandler<StaticObject>();
	attachSnakeCollisionHandler<BasicFood>();
	attachSnakeCollisionHandler<Snake>();
	attachSnakeCollisionHandler<Ant>();
}

void SnakeWndProc::detachCollisionHandlers() {
	detachSnakeCollisionHandler<StaticObject>();
	detachSnakeCollisionHandler<BasicFood>();
	detachSnakeCollisionHandler<Snake>();
	detachSnakeCollisionHandler<Ant>();
}

void SnakeWndProc::snakeNewElementAddedHandler(Snake *snake) {
	if (snake != smMap.getSnake())
		return;

	sdDisp.addScore(1);
}

void SnakeWndProc::adjustWindowSize(HWND hWnd) {
	RECT rcWnd;
	rcWnd.left = 0;
	rcWnd.top = 0;
	rcWnd.right = smMap.getWidthPixels();
	rcWnd.bottom = smMap.getHeightPixels() + sdDisp.getHeight();
	ClientToScreen(hWnd, (LPPOINT)&rcWnd);
	ClientToScreen(hWnd, (LPPOINT)&rcWnd.right);

	AdjustWindowRect(&rcWnd, WS_OVERLAPPEDWINDOW, FALSE);

	SetWindowPos(hWnd, nullptr, 0, 0, rcWnd.right - rcWnd.left, rcWnd.bottom - rcWnd.top, SWP_NOZORDER | SWP_NOMOVE);
}

void SnakeWndProc::initializeGame(HWND hWnd) {
	BasicFood::loadBitmaps("graphics\\apple.bmp");
	Ant::loadAntBitmaps("graphics\\ant.bmp");
	initCollisionHandlers();
	adjustWindowSize(hWnd);
	cvmCursor.hideCursor();
	RedrawWindow(hWnd, nullptr, nullptr, RDW_INVALIDATE);
}
