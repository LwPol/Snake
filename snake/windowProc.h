#pragma once

#include "stdafx.h"

class WindowProc {
	private:
		friend class Application;

		static WindowProc *instance;

	protected:
		WindowProc() = default;
		WindowProc(const WindowProc&) = default;
		WindowProc(WindowProc&&) = default;
		virtual ~WindowProc() = default;
	public:
		virtual LRESULT wndProc(HWND, UINT, WPARAM, LPARAM) = 0;
};