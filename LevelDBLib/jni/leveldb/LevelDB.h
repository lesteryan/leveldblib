/*
 * LevelDB.h
 *
 *  Created on: 2016年7月29日
 *      Author: yanxi
 */

#ifndef LEVELDB_LEVELDB_H_
#define LEVELDB_LEVELDB_H_

#include "leveldb/db.h"
#include "leveldb/write_batch.h"

#ifdef BUILD_HADOOP
#include "util/fileSystem/VirtualMemFile.h"
#endif

using namespace leveldb;


class LevelDB
{
public:
    static const char KEY_LENGTH = (sizeof(short) + 4);

	static const short KEY_HEAD_NAVILINK = 0x0001;
    static const short KEY_HEAD_ROADNAME = 0x0002;

	LevelDB();

	~LevelDB();

	Status open(const std::string& _dbPath, const std::string& _dbName, bool _readOnly, bool _writeOnly);

	Status open(const std::string& _dbPath);

	bool isOpen();

	Status insert(const Slice& key, const Slice& value);

    Status insert(const unsigned short type, const unsigned int id, const Slice& value);

    Status insert(const unsigned short type, const unsigned int id, const int index, const Slice& value);

	Status atomReady();

	Status atomCommit();

	Status query(const Slice& key, std::string& value);

    Status query(const unsigned short type, const unsigned int id, std::string& value);

    Status query(const unsigned short type, const unsigned int id, const int index, std::string& value);

	Status destory();

	Status make();

	void close();

#ifdef BUILD_HADOOP
	Status getVirtualFiles(std::vector<VirtualMemFile *>& _files);
#endif

private:
	DB* db;
	WriteBatch batch;
	Options option;

	std::string dbPath;
	std::string dbName;

    bool isAtomic;

    void integer2array(const long long value, char * dest, const int len);
    void makeKey(const short type, const int id, char * key);
    void makeKey(const short type, const int id, int index, char * key);
};


#endif /* LEVELDB_LEVELDB_H_ */
