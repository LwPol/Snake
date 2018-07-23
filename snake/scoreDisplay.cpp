#include "stdafx.h"
#include "scoreDisplay.h"

ScoreDisplay::ScoreDisplay(int width, int height) :
	width(-1),
	heigth(-1),
	timeCounterState(TimeCounterState::NOT_RUNNING)
{
	if (width < 0 || height < 0)
		throw std::invalid_argument("ScoreDisplay::ctor: Width and height cannot be negative");
	hdcDisplay = CreateCompatibleDC(nullptr);
	if (!hdcDisplay)
		throw ScoreDisplayException(ScoreDisplayException::CREATE_DC_FAIL);
	SetBkMode(hdcDisplay, TRANSPARENT);
	SetTextColor(hdcDisplay, RGB(255, 255, 255));
	
	SmartHandle<HFONT> hfScore = CreateFont(0, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, 0, L"Arial");
	if (!hfScore)
		throw ScoreDisplayException(ScoreDisplayException::CREATE_FONT_FAIL);
	hdcDisplay.setFont(std::move(hfScore));

	setSize(width, height);
}

ScoreDisplay::~ScoreDisplay() {
	stopTimer();
}

void ScoreDisplay::draw() {
	RECT rc = { 0, 0, width, heigth };
	FillRect(hdcDisplay, &rc, reinterpret_cast<HBRUSH>(GetStockObject(BLACK_BRUSH)));

	WCHAR buffer[128];
	wsprintf(buffer, L"Wynik: %d", score);
	SIZE scoreInfoSize;
	GetTextExtentPoint32(hdcDisplay, buffer, lstrlen(buffer), &scoreInfoSize);
	TextOut(hdcDisplay, 10, (heigth - scoreInfoSize.cy) / 2, buffer, lstrlen(buffer));

	int inGame = static_cast<int>(secondsInGame.load());
	int minutes = inGame / 60;
	int secondsRemainer = inGame - 60 * minutes;
	WCHAR sSeconds[3];
	wsprintf(sSeconds, L"%2d", secondsRemainer);
	if (sSeconds[0] == L' ')
		sSeconds[0] = L'0';
	wsprintf(buffer, L"Czas: %2d:%.2s", minutes, sSeconds);
	TextOut(hdcDisplay, 120, (heigth - scoreInfoSize.cy) / 2, buffer, lstrlen(buffer));
}

void ScoreDisplay::render(HDC hdc, int x, int y) {
	draw();
	BitBlt(hdc, x, y, width, heigth, hdcDisplay, 0, 0, SRCCOPY);
}

bool ScoreDisplay::startTimer() {
	if (timeCounter.joinable())
		return false;

	timeCounterState = TimeCounterState::NORMAL;
	secondsInGame.store(0.0);
	lastUpdate = std::chrono::system_clock::now();
	timeCounter = std::thread(&ScoreDisplay::updateTimer, this);
	return true;
}

void ScoreDisplay::stopTimer() {
	if (timeCounter.joinable()) {
		std::unique_lock<std::mutex> ul(stateMutex);
		timeCounterState = TimeCounterState::ABORT;
		ul.unlock();
		stateSync.notify_one();
		timeCounter.join();
	}
	timeCounterState = TimeCounterState::NOT_RUNNING;
}

void ScoreDisplay::pauseTimer() {
	std::lock_guard<std::mutex> lg(stateMutex);
	if (timeCounterState == TimeCounterState::NORMAL)
		timeCounterState = TimeCounterState::SUSPENDED;
}

void ScoreDisplay::unpauseTimer() {
	std::unique_lock<std::mutex> ul(stateMutex);
	if (timeCounterState == TimeCounterState::SUSPENDED) {
		timeCounterState = TimeCounterState::NORMAL;
		lastUpdate = std::chrono::system_clock::now();
		ul.unlock();
		stateSync.notify_one();
	}
}

void ScoreDisplay::setSize(int width, int height) {
	if (width < 0 || height < 0 || (width == this->width && height == this->heigth))
		return;

	SmartHandle<HBITMAP> hbmNew = CreateBitmap(width, height, 1, 32, nullptr);
	hdcDisplay.setBitmap(std::move(hbmNew));

	this->width = width;
	this->heigth = height;

	draw();
}

void ScoreDisplay::updateTimer() {
	bool bAbort = false;

	auto stateSyncPoint = [&] {
		if (timeCounterState == TimeCounterState::ABORT)
			bAbort = true;
		return timeCounterState != TimeCounterState::SUSPENDED;
	};

	while (!bAbort) {
		std::this_thread::sleep_for(std::chrono::milliseconds(100));

		auto current = std::chrono::system_clock::now();
		std::chrono::duration<double> timeElapsed = current - lastUpdate;
		lastUpdate = current;

		double val = secondsInGame.load();
		val += timeElapsed.count();
		secondsInGame.store(val);

		std::unique_lock<std::mutex> ul(stateMutex);
		stateSync.wait(ul, stateSyncPoint);
	}
}
