#pragma once

#include "stdafx.h"
#include "windowProc.h"
#include <type_traits>
#include <memory>
#include <utility>

class Application {
	private:
		static HINSTANCE hInst;
		static HWND hMainWindow;
		static WNDCLASSEX wcex;
		static DWORD dwWindowStyle;
	public:
		static ATOM registerWindowClass();

		static bool createMainWindow(int width, int height, LPCWSTR title, int nShowCmd = SW_SHOWNORMAL);

		static int run();

		static LRESULT CALLBACK baseWindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

		template <class T, class U, class ...Args>
		static void setNewWindowProc(U &&param, Args&& ...args) {
			static_assert(std::is_base_of<WindowProc, T>::value, "Template parameter must inherit from WindowProc class");

			if (std::is_base_of<WindowProc, typename std::remove_reference<U>::type>::value) {
				if (WindowProc::instance == reinterpret_cast<WindowProc*>(std::addressof(param))) {
					WindowProc::instance = new T(std::forward<U>(param), std::forward<Args>(args)...);
					delete std::addressof(param);
				}
				else {
					setNewWindowProc<void>();
					WindowProc::instance = new T(std::forward<U>(param), std::forward<Args>(args)...);
				}
			}
			else {
				setNewWindowProc<void>();
				WindowProc::instance = new T(std::forward<U>(param), std::forward<Args>(args)...);
			}
		}
		template <class T>
		static void setNewWindowProc() {
			static_assert(std::is_base_of<WindowProc, T>::value, "Template parameter must inherit from WindowProc class");

			setNewWindowProc<void>();
			WindowProc::instance = new T;
		}
		template <>
		static void setNewWindowProc<void>() {
			delete WindowProc::instance;
			WindowProc::instance = nullptr;
		}

		static void setIcon(HICON hIcon);
		static void setSmallIcon(HICON hIcon);
		static void setCursor(HCURSOR hCursor);
		static void setWindowStyle(DWORD style);
		static DWORD getWindowStyle();

		static HWND hWnd() { return hMainWindow; }
		static HINSTANCE hInstance() { return hInst; }
		static WindowProc* windowProcInst() { return WindowProc::instance; }
};
