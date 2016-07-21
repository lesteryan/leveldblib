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

using namespace std;

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

	bool read(size_t len, std::string& result);

	bool read(std::string& result, size_t pos, size_t len);

	bool write(const std::string& result, size_t len);

	bool write(size_t pos, size_t len, std::string& result);

	bool flush();

	bool static parseFilePath(const std::string& fname, std::string& filePath, std::string& fileName);

	void close();

	std::string toString();

private:
    std::string _filePath;
	std::string _fileName;
	std::string _fileContent;
	const int _fd;
	volatile size_t _filePos;
	pthread_mutex_t _mutex;

    bool parseFilePath(const std::string& file);
};

#endif /* VIRTUALMEMFILE_H_ */
