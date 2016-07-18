/*
 * VirtualMemFileSystem.h
 *
 *  Created on: 2016年7月15日
 *      Author: yanxi
 */

#ifndef VIRTUALMEMFILESYSTEM_H_
#define VIRTUALMEMFILESYSTEM_H_

#include <iostream>
#include <map>
#include <vector>
#include <stdlib.h>

#include "VirtualMemDir.h"

using namespace std;

class VirtualMemFileSystem {
public:
	VirtualMemFileSystem();

	~VirtualMemFileSystem();

	void clear();

	bool isContain(const std::string& dirName);

	VirtualMemDir* createDir(const std::string& dirName);

	VirtualMemDir * getDir(const std::string& dirName);

	bool deleteDir(const std::string& dirName);

	bool getChildrenName(const std::string& dirName, const std::string& dbname,
			std::vector<std::string>* result);

	std::string toString();

private:
	std::vector<VirtualMemDir *> _dirs;
};

#endif /* VIRTUALMEMFILESYSTEM_H_ */
