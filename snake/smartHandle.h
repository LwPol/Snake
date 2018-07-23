#include "stdafx.h"
#include <utility>
#include <memory>
#include <type_traits>

template <class T>
struct pointer_type {
	typedef T type;
};

template <class T>
struct pointer_type<T*> {
	typedef T type;
};

template <class T>
class SmartHandle {
	static_assert(std::is_same<typename std::remove_reference<T>::type, HANDLE>::value, "Template parameter is not a handle");

	std::unique_ptr<typename pointer_type<T>::type, decltype(&CloseHandle)> ptr;
	public:
		SmartHandle(T handle = nullptr, decltype(&CloseHandle) cleaningFunc = &CloseHandle) :
			ptr(handle, cleaningFunc) {}

		operator T() {
			return ptr.get();
		}
};

template <>
class SmartHandle<HBITMAP> {
	std::unique_ptr<pointer_type<HBITMAP>::type, decltype(&DeleteObject)> ptr;
	public:
		SmartHandle(HBITMAP hbmp = nullptr) :
			ptr(hbmp, &DeleteObject) {}

		operator HBITMAP() {
			return ptr.get();
		}
};

template <>
class SmartHandle<HBRUSH> {
	std::unique_ptr<pointer_type<HBRUSH>::type, decltype(&DeleteObject)> ptr;
	public:
		SmartHandle(HBRUSH hbr = nullptr) :
			ptr(hbr, &DeleteObject) {}

		operator HBRUSH() {
			return ptr.get();
		}
};

template <>
class SmartHandle<HPEN> {
	std::unique_ptr<pointer_type<HPEN>::type, decltype(&DeleteObject)> ptr;
	public:
		SmartHandle(HPEN hp = nullptr) :
			ptr(hp, &DeleteObject) {}

		operator HPEN() {
			return ptr.get();
		}
};

template <>
class SmartHandle<HFONT> {
	std::unique_ptr<pointer_type<HFONT>::type, decltype(&DeleteObject)> ptr;
	public:
		SmartHandle(HFONT hf = nullptr) :
			ptr(hf, &DeleteObject) {}

		operator HFONT() {
			return ptr.get();
		}
};

template <>
class SmartHandle<HDC> {
	std::unique_ptr<pointer_type<HDC>::type, decltype(&DeleteDC)> ptr;

	SmartHandle<HBITMAP> bitmap;
	SmartHandle<HBRUSH> brush;
	SmartHandle<HPEN> pen;
	SmartHandle<HFONT> font;
	public:
		SmartHandle(HDC hdc = nullptr) :
			ptr(hdc, &DeleteDC),
			bitmap((HBITMAP)GetCurrentObject(hdc, OBJ_BITMAP)),
			brush((HBRUSH)GetCurrentObject(hdc, OBJ_BRUSH)),
			pen((HPEN)GetCurrentObject(hdc, OBJ_PEN)),
			font((HFONT)GetCurrentObject(hdc, OBJ_FONT)) {}
	
		operator HDC() {
			return ptr.get();
		}

		void setBitmap(SmartHandle<HBITMAP> &newBitmap) {
			std::swap(bitmap, newBitmap);
			SelectObject(*this, bitmap);
		}
		void setBitmap(SmartHandle<HBITMAP> &&newBitmap) {
			SelectObject(*this, newBitmap);
			bitmap = std::move(newBitmap);
		}

		void setBrush(SmartHandle<HBRUSH> &newBrush) {
			std::swap(brush, newBrush);
			SelectObject(*this, brush);
		}
		void setBrush(SmartHandle<HBRUSH> &&newBrush) {
			SelectObject(*this, newBrush);
			brush = std::move(newBrush);
		}

		void setPen(SmartHandle<HPEN> &newPen) {
			std::swap(pen, newPen);
			SelectObject(*this, pen);
		}
		void setPen(SmartHandle<HPEN> &&newPen) {
			SelectObject(*this, newPen);
			pen = std::move(newPen);
		}

		void setFont(SmartHandle<HFONT> &newFont) {
			std::swap(font, newFont);
			SelectObject(*this, font);
		}
		void setFont(SmartHandle<HFONT> &&newFont) {
			SelectObject(*this, newFont);
			font = std::move(newFont);
		}
};
