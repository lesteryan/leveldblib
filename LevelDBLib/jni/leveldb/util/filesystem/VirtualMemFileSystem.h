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
#include <set>
#include <stdlib.h>

#include "VirtualMemFile.h"

using namespace std;

class VirtualMemFileSystem {
public:
	VirtualMemFileSystem();

	~VirtualMemFileSystem();

	void clear();

    bool createDir(const std::string& dirName);

    bool isContainDir(const std::string& dirName);

	bool deleteDir(const std::string& dirName);

    void renameDir(const std::string& originDirName, const std::string& newDirName);

	bool getChildrenName(const std::string& dirName, const std::string& dbname,
			std::vector<std::string>* result);

    VirtualMemFile* createFile(const std::string& fileName);

    bool deleteFile(const std::string& fileName);

    VirtualMemFile* getFile(const std::string fileName);

    VirtualMemFile* getFile(const int fd);

    bool isContainFile(const std::string fileName);

    void getChildrenName(const std::string& dbname,
            std::vector<std::string>* result);

    std::vector<VirtualMemFile *> getAllFile();

    void closeAll();

	std::string toString();

private:
    std::set<std::string > _dirs;
    std::vector<VirtualMemFile *> _files;

    // /mnt/sdcard -> /mnt/sdcard/   /mnt/sdcard/ -> /mnt/sdcard/
    std::string formatDir(const std::string& path);
};

#endif /* VIRTUALMEMFILESYSTEM_H_ */
