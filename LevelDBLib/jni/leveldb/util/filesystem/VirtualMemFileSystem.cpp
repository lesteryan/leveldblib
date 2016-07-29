/*
 * VirtualMemFileSystem.cpp
 *
 *  Created on: 2016年7月15日
 *      Author: yanxi
 */

#include <util/LogUtil.h>
#include "VirtualMemFileSystem.h"
using namespace std;

VirtualMemFileSystem::VirtualMemFileSystem() {
}

VirtualMemFileSystem::~VirtualMemFileSystem() {
	LOGW("VirtualMemFileSystem destructor");
    for (std::vector<VirtualMemFile *>::iterator iterator = _files.begin();
            iterator != _files.end(); iterator++) {
        iterator = _files.erase(iterator);
		delete (*iterator);
	}
    _files.clear();
    _dirs.clear();
}

void VirtualMemFileSystem::clear() {
	LOGW("clear file system");
	_dirs.clear();
}

bool VirtualMemFileSystem::isContainDir(const std::string& dirName) {
    std::string _dirName = formatDir(dirName);
    std::set<std::string>::iterator iterator = _dirs.find(_dirName);

    return iterator != _dirs.end();
}

bool VirtualMemFileSystem::createDir(const std::string& dirName) {
    std::string _dirName = formatDir(dirName);
    if (isContainDir(_dirName))
        return false;

    _dirs.insert(_dirName);

    return true;
}

bool VirtualMemFileSystem::deleteDir(const std::string& dirName) {
    std::string _dirName = formatDir(dirName);

    for (std::vector<VirtualMemFile *>::iterator iterator = _files.begin();
            iterator != _files.end(); ) {
        if ((*iterator)->getDirName() == dirName) {
                delete (*iterator);
                iterator = _files.erase(iterator);
        }
        else
        {
            iterator++;
        }
    }

    std::set<std::string>::iterator iterator = _dirs.find(_dirName);
    if(iterator == _dirs.end())
    {
        return false;
    }

    _dirs.erase(iterator);

    return true;
}

bool VirtualMemFileSystem::getChildrenName(const std::string& dirName,
		const std::string& dbname, std::vector<std::string>* result) {
	result->clear();

    if(this->isContainDir(dirName) == false)
        return false;

    std::string _dirName = formatDir(dirName);
    for(int i = 0 ; i < _files.size() ; i++)
        if(_files[i]->getDirName() == _dirName)
            result->push_back(_files[i]->getFileName());


	return true;
}

void VirtualMemFileSystem::renameDir(const std::string& originDirName, const std::string& newDirName) {
    std::string _originDirName = formatDir(originDirName);
    std::set<std::string>::iterator iterator = _dirs.find(_originDirName);
    if(iterator == _dirs.end())
        return ;
    else
    {
        _dirs.erase(iterator);
        _dirs.insert(formatDir(newDirName));
    }
}

VirtualMemFile* VirtualMemFileSystem::createFile(const std::string& fileName) {
	LOGI("create file %s", fileName.data());

	if(this->isContainFile(fileName))
	{
		VirtualMemFile* file = getFile(fileName);
		file->seek(0);

		LOGI("contain %s, needn't create", fileName.data());

		return file;
	}

    VirtualMemFile * file = new VirtualMemFile(fileName);

    _files.push_back(file);
    _dirs.insert(file->getDirName());

    return file;
}

bool VirtualMemFileSystem::deleteFile(const std::string& fileName) {
	LOGI("delete file %s", fileName.data());
	std::string _filePath, _fileName;
	if(VirtualMemFile::parseFilePath(fileName, _filePath, _fileName) == false)
		return false;

    for (std::vector<VirtualMemFile *>::iterator iterator = _files.begin();
            iterator != _files.end(); ) {
        if ((*iterator)->getDirName()  == _filePath && (*iterator)->getFileName() == _fileName) {
                delete (*iterator);
                _files.erase(iterator);

                return true;
        }
        else
        {
            iterator++;
        }
    }

    return false;
}

VirtualMemFile* VirtualMemFileSystem::getFile(const std::string fileName) {
	std::string _filePath, _fileName;
	if(VirtualMemFile::parseFilePath(fileName, _filePath, _fileName) == false)
		return NULL;

    for (int i = 0; i < _files.size(); i++) {
        if (_files[i]->getDirName() == _filePath && _files[i]->getFileName() == _fileName) {
            return _files[i];
        }
    }

    return NULL;
}

VirtualMemFile* VirtualMemFileSystem::getFile(const int fd) {
    for (int i = 0; i < _files.size(); i++) {
        if (_files[i]->getFd() == fd) {
            return _files[i];
        }
    }

    return NULL;
}

bool VirtualMemFileSystem::isContainFile(const std::string fileName) {
	std::string _filePath, _fileName;
	if(VirtualMemFile::parseFilePath(fileName, _filePath, _fileName) == false)
		return false;

//	LOGI("_file size %d", _files.size());
    for (int i = 0; i < _files.size(); i++) {
    	{
//    		LOGI("compare %s %s, %s %s,", _files[i]->getDirName().data() , _filePath.data() , _files[i]->getFileName().data() , _fileName.data());
			if (_files[i]->getDirName() == _filePath && _files[i]->getFileName() == _fileName)
				return true;
    	}
    }

    return false;
}

std::string VirtualMemFileSystem::toString() {
	std::string buffer;

    buffer = "\ndirs:\n";

    for(std::set<std::string >::iterator iterator = _dirs.begin() ; iterator != _dirs.end() ; iterator++)
        buffer += "\t" + (*iterator) + "\n";

    buffer += "files\n";

    for (int i = 0; i < _files.size(); i++)
        buffer += _files[i]->toString() + "\n";

	return buffer;
}

std::string VirtualMemFileSystem::formatDir(const std::string &dir)
{
    if(dir.length() <= 1)
        return std::string();

    std::string _dir = dir;

    for(int i = 0 ; i < _dir.length() ; i++)
        if(_dir[i] == '\\')
            _dir[i] = '/';

    if(_dir[_dir.length() - 1] != '/')
        _dir = _dir + "/";

    if(_dir[dir.length() - 2] == '/')
    	_dir = _dir.substr(0, _dir.length() - 1);

    return _dir;
}

void VirtualMemFileSystem::closeAll()
{
	for(int i = 0 ; i < _files.size() ; i++)
	{
		if(_files[i] != NULL)
			_files[i]->close();
	}
}

std::vector<VirtualMemFile *> VirtualMemFileSystem::getAllFile()
{
	return _files;
}
