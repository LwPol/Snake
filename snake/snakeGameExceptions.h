#pragma once

#include <exception>
#include <string>
#include <sstream>

class BitmapLoadFail : public std::exception {
	std::string errorMsg;
	public:
		BitmapLoadFail(const std::string &msg, int errorCode = 0) {
			errorMsg = msg;
			if (errorCode) {
				errorMsg += "\r\n";
				std::stringstream ss;
				ss << "Kod b³êdu: ";
				ss << errorCode;
				errorMsg += ss.str();
			}
		}

		const char* what() const override {
			return errorMsg.c_str();
		}
};

class SnakeElementAdditionException : public std::exception {
	public:
		const char* what() const override {
			return "Cannot add new element to snake as tail's position is occupied";
		}
};

class GdiObjectCreationException : public std::exception {
	public:
		const char* what() const override {
			return "Failed to create a GDI object";
		}
};