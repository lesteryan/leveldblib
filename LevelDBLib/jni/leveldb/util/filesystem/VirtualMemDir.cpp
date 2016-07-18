/*
 * VirtualMemDir.cpp
 *
 *  Created on: 2016年7月15日
 *      Author: yanxi
 */

#include "VirtualMemDir.h"

using namespace std;

VirtualMemDir::VirtualMemDir() {
}

VirtualMemDir::VirtualMemDir(const std::string& dirName) :
		_dirName(dirName) {
}

VirtualMemDir::~VirtualMemDir() {
	for (std::vector<VirtualMemFile *>::iterator iterator = _files.begin();
			iterator != _files.end(); iterator++) {
		iterator = _files.erase(iterator);
		delete (*iterator);
	}
}

std::string VirtualMemDir::getDirName() {
	return _dirName;
}

void VirtualMemDir::rename(const std::string& dirName) {
	_dirName = dirName;
}

VirtualMemFile* VirtualMemDir::createFile(const std::string& fileName) {
	if (isContain(fileName))
		return NULL;

	VirtualMemFile * file = new VirtualMemFile(fileName);
	_files.push_back(file);

	return file;
}

bool VirtualMemDir::deleteFile(const std::string& fileName) {
	if (!isContain(fileName))
		return false;

	for (std::vector<VirtualMemFile *>::iterator iterator = _files.begin();
			iterator != _files.end(); iterator++) {
		if ((*iterator)->getFileName() == fileName) {
			{
				iterator = _files.erase(iterator);
				delete (*iterator);
				break;
			}
		}
	}

	return true;
}

VirtualMemFile* VirtualMemDir::getFile(const std::string fileName) {
	for (int i = 0; i < _files.size(); i++) {
		if (_files[i]->getFileName() == fileName) {
			return _files[i];
		}
	}

	return NULL;
}

bool VirtualMemDir::isContain(const std::string fileName) {
	for (int i = 0; i < _files.size(); i++) {
		if (_files[i]->getFileName() == fileName)
			return true;
	}

	return false;
}

void VirtualMemDir::clear() {
	_files.clear();
}

void VirtualMemDir::getChildrenName(const std::string& dbname,
		std::vector<std::string>* result) {
	for (int i = 0; i < _files.size(); i++) {
		if (_files[i]->getFileName().substr(0, dbname.length() - 1) == dbname)
			result->push_back(_files[i]->getFileName());
	}
}

std::string VirtualMemDir::toString() {
	std::string buffer = std::string("dirName ") + _dirName + "\n files: "
			+ "\n";
	for (int i = 0; i < _files.size(); i++)
		buffer += " " + _files[i]->toString() + "\n";

	return buffer;
}
