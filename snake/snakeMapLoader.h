#pragma once

#include "stdafx.h"

class SnakeMap;

class SnakeMapLoader {
	private:
		static constexpr DWORD magic = 0x4D53574C; // LWSM

		struct MapElement {
			DWORD type;
			int x;
			int y;
		};

		static bool addNewElement(SnakeMap &map, const MapElement &element);
		template <class T>
		static void readDataFromFile(std::ifstream &inputFile, T &read, const std::string &inputFileName) {
			if (!inputFile.read(reinterpret_cast<char*>(&read), sizeof(T)))
				throw SnakeMapLoadingException(SnakeMapLoadingException::FILE_READING_ERROR, inputFileName);
		}
		template <class T>
		static std::ostream& writeDataToFile(std::ofstream &outputFile, const T &write) {
			return outputFile.write(reinterpret_cast<const char*>(&write), sizeof(T));
		}
	public:
		static SnakeMap loadMapFromFile(const std::string &sPath);
		static bool saveMapToFile(const SnakeMap &map, const std::string &sPath, bool overwrite);


		struct SnakeMapLoadingException : public std::exception {
			std::string fileName;
			enum Reason {
				CANNOT_OPEN_FILE,
				INVALID_FILE_FORMAT,
				FILE_READING_ERROR,
				INVALID_MAP_SIZE,
				INVALID_DATA_IN_FILE
			} reason;
			std::string msg;

			SnakeMapLoadingException(Reason reason, const std::string &fileName) :
				reason(reason),
				fileName(fileName)
			{
				switch (reason) {
					case CANNOT_OPEN_FILE:
						msg = std::string("Failed to open file ") + fileName;
						break;
					case INVALID_FILE_FORMAT:
						msg = std::string("File ") + fileName + std::string(" is not a valid snake map file");
						break;
					case FILE_READING_ERROR:
						msg = std::string("Error occurred while reading file ") + fileName;
						break;
					case INVALID_MAP_SIZE:
						msg = std::string("Map size specified in ") + fileName + std::string(" is invalid");
						break;
					case INVALID_DATA_IN_FILE:
						msg = std::string("Data in ") + fileName + std::string(" are invalid");
						break;
					default:
						msg = "Unknown exception";
				}
			}

			const char* what() const override {
				return msg.c_str();
			}
		};

		struct SnakeMapSavingException : public std::exception {
			enum Reason {
				INVALID_FILE_NAME,
				FIND_FILE_FAILED,
				CANNOT_CREATE_FILE,
				FILE_WRITING_ERROR
			} reason;

			SnakeMapSavingException(Reason reason) :
				reason(reason)
			{
			}

			const char* what() const override {
				switch (reason) {
					case INVALID_FILE_NAME:
						return "Given file name is invalid";
					case FIND_FILE_FAILED:
						return "Internal error";
					case CANNOT_CREATE_FILE:
						return "Failed to create a file";
					case FILE_WRITING_ERROR:
						return "Error occurred while writing to a file";
					default:
						return "Unknown exception";
				}
			}
		};
};