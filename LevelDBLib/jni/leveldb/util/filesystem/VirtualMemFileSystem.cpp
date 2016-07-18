/*
 * VirtualMemFileSystem.cpp
 *
 *  Created on: 2016年7月15日
 *      Author: yanxi
 */

#include "VirtualMemFileSystem.h"

using namespace std;

VirtualMemFileSystem::VirtualMemFileSystem() {
}

VirtualMemFileSystem::~VirtualMemFileSystem() {
	for (std::vector<VirtualMemDir *>::iterator iterator = _dirs.begin();
			iterator != _dirs.end(); iterator++) {
		iterator = _dirs.erase(iterator);
		delete (*iterator);
	}
}

void VirtualMemFileSystem::clear() {
	_dirs.clear();
}

bool VirtualMemFileSystem::isContain(const std::string& dirName) {
	for (int i = 0; i < _dirs.size(); i++) {
		if (_dirs[i]->getDirName() == dirName) {
			return true;
		}
	}

	return false;
}

VirtualMemDir* VirtualMemFileSystem::createDir(const std::string& dirName) {
	if (isContain(dirName))
		return NULL;

	VirtualMemDir * dir = new VirtualMemDir(dirName);
	_dirs.push_back(dir);

	return dir;
}

VirtualMemDir * VirtualMemFileSystem::getDir(const std::string& dirName) {
	for (int i = 0; i < _dirs.size(); i++) {
		if (_dirs[i]->getDirName() == dirName) {
			return _dirs[i];
		}
	}

	return NULL;
}

bool VirtualMemFileSystem::deleteDir(const std::string& dirName) {
	if (!isContain(dirName))
		return false;

	for (std::vector<VirtualMemDir *>::iterator iterator = _dirs.begin();
			iterator != _dirs.end(); iterator++) {
		if ((*iterator)->getDirName() == dirName) {
			{
				iterator = _dirs.erase(iterator);
				delete (*iterator);
				break;
			}
		}
	}

	return true;
}

bool VirtualMemFileSystem::getChildrenName(const std::string& dirName,
		const std::string& dbname, std::vector<std::string>* result) {
	result->clear();
	VirtualMemDir *dir;
	if ((dir = getDir(dirName)) == NULL)
		return false;

	dir->getChildrenName(dbname, result);
	return true;
}

std::string VirtualMemFileSystem::toString() {
	std::string buffer;
	for (int i = 0; i < _dirs.size(); i++)
		buffer += _dirs[i]->toString() + "\n";

	return buffer;
}
