/*
 * VirtualMemFile.cpp
 *
 *  Created on: 2016年7月15日
 *      Author: yanxi
 */

#include <stdlib.h>
#include "VirtualMemFile.h"
#include "FdManager.h"

using namespace std;

VirtualMemFile::VirtualMemFile() :
		_fd(FdManager::allocFd()), _filePos(0), _lock(false) {
}

VirtualMemFile::VirtualMemFile(const std::string& fileName) :
		_fd(FdManager::allocFd()), _fileName(fileName), _filePos(0), _lock(false) {
}

VirtualMemFile::~VirtualMemFile()
{
	FdManager::recycleFd(_fd);
}

std::string VirtualMemFile::getFileName() {
	return _fileName;
}

int VirtualMemFile::getFd() {
	return _fd;
}

void VirtualMemFile::rename(std::string newName) {
	_fileName = newName;
}

bool VirtualMemFile::lockFile() {
	if(_lock)
		return false;
	_lock = true;

	return true;
}

bool VirtualMemFile::unlockFile() {

	return true;
}

bool VirtualMemFile::lockOrUnlockFile()
{
	_lock = !_lock;
	return true;
}

size_t VirtualMemFile::getFileSize() {
	return _fileContent.size();
}

bool VirtualMemFile::skip(uint64_t n) {
	if (_filePos + n >= getFileSize())
		return false;

	_filePos += n;
	return true;
}

bool VirtualMemFile::seek(uint64_t n) {
	if (n > getFileSize())
		return false;

	_filePos = n;
	return true;
}

bool VirtualMemFile::read(size_t len, std::string& result) {
	if (_filePos + len > getFileSize())
		return false;

	result = _fileContent.substr(_filePos, len);
	_filePos += len;

	return true;
}

bool VirtualMemFile::read(std::string& result, size_t pos, size_t len) {
	if (pos + len > getFileSize())
		return false;

	result = _fileContent.substr(pos, len);
	return true;
}

bool VirtualMemFile::write(const std::string& result, size_t len) {
	_fileContent += result;
	_filePos += len;

	return true;
}

bool VirtualMemFile::write(size_t pos, size_t len, std::string& result) {
	if (pos > getFileSize())
		return false;
	else if (pos + len > getFileSize()) {
		_fileContent = _fileContent.substr(0, pos) + result;
		_filePos = pos + len;
	} else {
		_fileContent.replace(pos, len, result);
		_filePos = pos + len;
	}

	return true;
}

bool VirtualMemFile::flush() {
	return true;
}

std::string VirtualMemFile::toString() {
	char fdString[8];
	sprintf(fdString, "%d", _fd);
	return std::string("\tfilename\t") + _fileName + "\t" + fdString + "\tfileContent\t"
			+ _fileContent;
}
