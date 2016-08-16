/*
 * VirtualMemFile.h
 *
 *  Created on: 2016年7月15日
 *      Author: yanxi
 */

#ifndef VIRTUALMEMFILE_H_
#define VIRTUALMEMFILE_H_

#include <iostream>
#include <map>
#include <vector>
#include <stdlib.h>
#include "leveldb/slice.h"

namespace leveldb_navi {

class VirtualMemFile {
public:
	VirtualMemFile(const std::string& fileName);

    ~VirtualMemFile();

	std::string getFileName();

    std::string getDirName();

	int getFd();

	void rename(const std::string& newName);

	bool lockFile();

	bool unlockFile();

	size_t getFileSize();

	bool skip(uint64_t n);

	bool seek(uint64_t n);

	size_t read(leveldb_navi::Slice* result, size_t len);

    size_t read(leveldb_navi::Slice* result, size_t pos, size_t len);

	bool write(const char* result, size_t len);

	bool write(size_t pos, size_t len, const char * result);

	bool flush();

	void close();

	void clean();

	std::string toString();

private:
    std::string _filePath;
	std::string _fileName;
//	std::string _fileContent;
	char * _fileContent;
	size_t _fileLength;
	const int _fd;
	size_t _filePos;
	pthread_mutex_t _mutex;

    void allocMemory(int writeLen);
};
}

#endif /* VIRTUALMEMFILE_H_ */
