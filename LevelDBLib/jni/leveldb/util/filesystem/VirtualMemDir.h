/*
 * VirtualMemDir.h
 *
 *  Created on: 2016年7月15日
 *      Author: yanxi
 */

#ifndef VIRTUALMEMDIR_H_
#define VIRTUALMEMDIR_H_

#include <iostream>
#include <map>
#include <vector>
#include <stdlib.h>

#include "VirtualMemFile.h"

using namespace std;

class VirtualMemDir {
public:

	VirtualMemDir();

	VirtualMemDir(const std::string& dirName);

	~VirtualMemDir();

	std::string getDirName();

	void rename(const std::string& dirName);

	VirtualMemFile* createFile(const std::string& fileName);

	bool deleteFile(const std::string& fileName);

	VirtualMemFile* getFile(const std::string fileName);

	bool isContain(const std::string fileName);

	void getChildrenName(const std::string& dbname,
			std::vector<std::string>* result);

	void clear();

	std::string toString();

private:
	std::string _dirName;
	std::vector<VirtualMemFile *> _files;
};

#endif /* VIRTUALMEMDIR_H_ */
