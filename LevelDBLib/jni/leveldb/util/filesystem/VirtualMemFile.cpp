/*
 * VirtualMemFile.cpp
 *
 *  Created on: 2016年7月15日
 *      Author: yanxi
 */

#include "VirtualMemFile.h"
#include "FdManager.h"
#include "../PathParser.h"
#include <pthread.h>

namespace leveldb_navi {
    
VirtualMemFile::VirtualMemFile(const std::string& fileName) :
        _fd(FdManager::allocFd()), _filePos(0), _fileLength(0){
	PathParser::parseFilePath(fileName, _filePath, _fileName);
		 pthread_mutex_init(&_mutex, 0);
    _fileContent = (char *)malloc(64);
    _fileLength = 0;
    _filePos = 0;
}

VirtualMemFile::~VirtualMemFile(){
    FdManager::recycleFd(this->getFd());
    pthread_mutex_destroy(&_mutex);
    free(_fileContent);
    close();
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
	PathParser::parseFilePath(newName, _filePath, _fileName);
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
	return _fileLength;
}

bool VirtualMemFile::skip(uint64_t n) {

	if (_filePos + n >= getFileSize())
		return false;
	else
		_filePos += n;
	return true;
}

bool VirtualMemFile::seek(uint64_t n) {
	if(n >= getFileSize())
		return false;

	_filePos = n;
	return true;
}

size_t VirtualMemFile::read(leveldb_navi::Slice* result, size_t len) {
	assert(len >= 0);
//	LOGI("read1 %s filepos = %ld, size = %ld, len = %ld", _fileName.data(), _filePos, _fileLength, len);
	if(len + _filePos > getFileSize())
		len = getFileSize() - _filePos;
	len = len < 0 ? 0 : len;

	*result = leveldb_navi::Slice(_fileContent + _filePos, len);

	_filePos += len;

//	if(result->size() != len)
//		LOGE("read error 1");
//	else if(result->size() == 0)
//		LOGE("read error 3, file pos %d", _filePos);

	return len;
}

size_t VirtualMemFile::read(leveldb_navi::Slice* result, size_t pos, size_t len) {
	assert(len >= 0);
//	LOGI("read2 %s filepos = %d, size = %ld, len = %ld. pos=%ld", _fileName.data(), _filePos, _fileLength, len, pos);
	if(pos + len > getFileSize())
	{
		len = getFileSize() - pos;
	}

	len = len < 0 ? 0 : len;

	*result = leveldb_navi::Slice(_fileContent + pos, len);
	_filePos = pos + len;

//	if(result->size() != len)
//		LOGE("read error 2");
//	else if(result->size() == 0)
//		LOGE("read error 4");

	return len;
}

//append
bool VirtualMemFile::write(const char * result, size_t len) {
	assert(len >= 0);
//	LOGI("write1 %s filepos = %ld, data = %s, size = %ld", _fileName.data(), _filePos, result, len);

	allocMemory(len);

	memcpy(_fileContent + _fileLength, result, len);

	_fileLength += len;
	_filePos = _fileLength;

	return true;
}

bool VirtualMemFile::write(size_t pos, size_t len, const char * result) {
	assert(len >= 0);
//	LOGI("write2 %s filepos = %ld, data = %s, pos = %ld, size = %ld", _fileName.data(), _filePos, result, pos, len);
	if (pos > getFileSize())
	{
		return false;
	}
	else
	{
		if(_filePos + len > sizeof(_fileContent))
			allocMemory(len);

		memcpy(_fileContent + pos, result, len);
		if(pos + len > getFileSize())
		{
			_fileLength = pos + len;
		}

		_filePos = pos + len;
	}

	return true;
}

bool VirtualMemFile::flush() {
	return true;
}

std::string VirtualMemFile::toString() {
	char temp[512];

	sprintf(temp, "\tfilePath %s\t fileName %s\t fileSize %d\tfileContent%s", _filePath.data(), _fileName.data(), getFileSize(), _fileContent);
	return std::string(temp, sizeof(temp));
}

void VirtualMemFile::close()
{
	_filePos = 0;
}

void VirtualMemFile::clean()
{
	_filePos = 0;
	_fileLength = 0;
	memset(_fileContent, 0 ,sizeof(_fileContent));
}

void VirtualMemFile::allocMemory(int writeLen)
{
	if(getFileSize() + writeLen < sizeof(_fileContent))
		return;

	int newSize = sizeof(_fileContent) * 2;
	while(newSize < getFileSize() + writeLen)
		newSize *= 2;

	char * temp = (char *)malloc(newSize);
	memcpy(temp, _fileContent, getFileSize());
	free(_fileContent);
	_fileContent = temp;
}
}
