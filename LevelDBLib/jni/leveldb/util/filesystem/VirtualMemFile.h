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
	VirtualMemFile();

	VirtualMemFile(const std::string& fileName);

	~VirtualMemFile();

	std::string getFileName();

	int getFd();

	void rename(std::string newName);

	bool lockFile();

	bool unlockFile();

	bool lockOrUnlockFile();

	size_t getFileSize();

	bool skip(uint64_t n);

	bool seek(uint64_t n);

	bool read(size_t len, std::string& result);

	bool read(std::string& result, size_t pos, size_t len);

	bool write(const std::string& result, size_t len);

	bool write(size_t pos, size_t len, std::string& result);

	bool flush();

	std::string toString();

private:
	const int _fd;
	std::string _fileName;
	std::string _fileContent;

	volatile uint64_t _filePos;
	volatile bool _lock;
};

#endif /* VIRTUALMEMFILE_H_ */
