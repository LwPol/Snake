#pragma once

#include "stdafx.h"

class ControlBase {
	protected:
		HWND myWindow;

		virtual LPCWSTR className() const = 0;
		virtual LRESULT handleMessage(UINT msg, WPARAM wParam, LPARAM lParam) = 0;
	public:
		ControlBase();
		ControlBase(const ControlBase&) = delete;
		ControlBase(ControlBase &&other);

		ControlBase& operator=(const ControlBase&) = delete;
		ControlBase& operator=(ControlBase &&other);

		virtual ~ControlBase();

		static LRESULT CALLBACK windowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

		bool create(LPCWSTR sWindowName,
			int x, int y,
			int width, int height,
			HWND hParent,
			int ctrlId = 0,
			DWORD dwStyle = WS_CHILD | WS_VISIBLE,
			DWORD dwExStyle = 0);

		void dispose();

		int getControlWidth() const;

		int getControlHeight() const;

		void setClassCursor(HCURSOR hCursor);

		HWND window() const { return myWindow; }
};