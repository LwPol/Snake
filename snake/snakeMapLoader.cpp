#include "stdafx.h"
#include "snakeMapLoader.h"
#include "snakeMap.h"
#include "basicTile.h"

SnakeMap SnakeMapLoader::loadMapFromFile(const std::string &sPath) {
	// get file's name from path
	std::string sFileName;
	std::size_t slashPos = sPath.find_last_of('\\');
	if (slashPos == std::string::npos)
		slashPos = sPath.find_last_of('/');
	if (slashPos == std::string::npos)
		sFileName = sPath;
	else
		sFileName = sPath.substr(slashPos + 1);

	// open file for reading
	std::ifstream file(sPath, std::ios::binary);
	if (!file.is_open())
		throw SnakeMapLoadingException(SnakeMapLoadingException::CANNOT_OPEN_FILE, sFileName);

	// verify file signature
	DWORD signature;
	readDataFromFile(file, signature, sFileName);
	if (signature != magic)
		throw SnakeMapLoadingException(SnakeMapLoadingException::INVALID_FILE_FORMAT, sFileName);

	// retrieve width and height of the map
	int width, height;
	readDataFromFile(file, width, sFileName);
	readDataFromFile(file, height, sFileName);
	if (width < SnakeGameConstants::mapMinWidth ||
		width > SnakeGameConstants::mapMaxWidth ||
		height < SnakeGameConstants::mapMinHeight ||
		height > SnakeGameConstants::mapMaxHeight)
	{
		throw SnakeMapLoadingException(SnakeMapLoadingException::INVALID_MAP_SIZE, sFileName);
	}

	// create map and add all elements
	SnakeMap ret(width, height);
	MapElement el;
	try {
		while (1) {
			readDataFromFile(file, el, sFileName);
			addNewElement(ret, el);
		}
	}
	catch (SnakeMapLoadingException &ex) {
		if (!file.eof())
			throw ex;
	}
	catch (std::invalid_argument&) {
		throw SnakeMapLoadingException(SnakeMapLoadingException::INVALID_DATA_IN_FILE, sFileName);
	}
	return ret;
}

bool SnakeMapLoader::saveMapToFile(const SnakeMap &map, const std::string &sPath, bool overwrite) {
	// verify that path is correct for FindFirstFile function and throw exception if isn't
	if (sPath.empty() || sPath[sPath.size() - 1] == '\\')
		throw SnakeMapSavingException(SnakeMapSavingException::INVALID_FILE_NAME);

	if (!overwrite) {
		// check if file with given name exists
		WIN32_FIND_DATAA fileData;
		HANDLE hSearch = FindFirstFileA(sPath.c_str(), &fileData);
		if (hSearch != INVALID_HANDLE_VALUE) {
			// file exists
			FindClose(hSearch);
			return false;
		}
		if (GetLastError() != ERROR_FILE_NOT_FOUND) {
			if (GetLastError() == ERROR_PATH_NOT_FOUND)
				throw SnakeMapSavingException(SnakeMapSavingException::INVALID_FILE_NAME);
			else
				throw SnakeMapSavingException(SnakeMapSavingException::FIND_FILE_FAILED);
		}
		// at this point we know that file doesn't exist
	}
	
	std::ofstream mapFile(sPath, std::ios::binary);
	if (!mapFile.is_open())
		throw SnakeMapSavingException(SnakeMapSavingException::CANNOT_CREATE_FILE);

	writeDataToFile(mapFile, magic);
	writeDataToFile(mapFile, map.mapWidth);
	writeDataToFile(mapFile, map.mapHeight);
	for (auto &obj : map.staticObjsCollection) {
		MapElement me;
		me.type = 0;
		me.x = obj->getPosX();
		me.y = obj->getPosY();
		writeDataToFile(mapFile, me);
	}
	if (!mapFile)
		throw SnakeMapSavingException(SnakeMapSavingException::FILE_WRITING_ERROR);

	return true;
}

bool SnakeMapLoader::addNewElement(SnakeMap &map, const MapElement &element) {
	switch (element.type) {
		case 0:
			return map.addObject<BasicTile>(element.x, element.y);
	}
	return false;
}
