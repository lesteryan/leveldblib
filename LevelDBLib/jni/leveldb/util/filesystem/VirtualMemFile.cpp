/*
 * VirtualMemFile.cpp
 *
 *  Created on: 2016年7月15日
 *      Author: yanxi
 */

#include "VirtualMemFile.h"
#include "FdManager.h"

#include <pthread.h>
using namespace std;

VirtualMemFile::VirtualMemFile(const std::string& fileName) :
        _fd(FdManager::allocFd()), _filePos(0), _mutex(PTHREAD_MUTEX_INITIALIZER) {
    parseFilePath(fileName);
}

VirtualMemFile::~VirtualMemFile(){
    FdManager::recycleFd(this->getFd());
    pthread_mutex_destroy(&_mutex);
}

std::string VirtualMemFile::getFileName() {
	return _fileName;
}

std::string VirtualMemFile::getDirName() {
    return _filePath;
}

int VirtualMemFile::getFd() {
	return _fd;
}

void VirtualMemFile::rename(const std::string& newName) {
	_fileName = newName;
}

bool VirtualMemFile::lockFile() {
	if(pthread_mutex_lock(&_mutex) == 0)
		return true;
	else
		return false;
}

bool VirtualMemFile::unlockFile() {
	if(pthread_mutex_unlock(&_mutex) == 0)
		return true;
	else
		return false;
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
    return std::string("\tfilePath\t") + _filePath +
           std::string("\tfilename\t") + _fileName +
           std::string("\tfileContent\t") + _fileContent;
}

bool VirtualMemFile::parseFilePath(const std::string& file)
{
    int lastSep = file.find_last_of('/');
    if(lastSep == std::string::npos)
        return false;

    this->_filePath = file.substr(0, lastSep + 1);
    this->_fileName = file.substr(lastSep + 1, file.length() - lastSep);
    return true;
}

bool VirtualMemFile::parseFilePath(const std::string& fname, std::string& filePath, std::string& fileName)
{
    int lastSep = fname.find_last_of('/');
    if(lastSep == std::string::npos)
        return false;

    filePath = fname.substr(0, lastSep + 1);
    fileName = fname.substr(lastSep + 1, fname.length() - lastSep);
    return true;
}
