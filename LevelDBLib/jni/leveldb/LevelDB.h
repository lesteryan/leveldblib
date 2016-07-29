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
	const char KEY_LENGTH = 10;

	LevelDB();

	~LevelDB();

	Status open(const std::string& _dbPath, const std::string& _dbName, bool _readOnly, bool _writeOnly);

	Status insert(const Slice& key, const Slice& value);

	Status atomReady();

	Status atomInsert(const Slice& key, const Slice& value);

	Status atomCommit();

	Status query(const Slice& key, std::string& value);

	Status destory();

	static void makeKey(unsigned short type, unsigned long long key, Slice& slice);

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

	static void integer2array(unsigned long long value, unsigned char * dest, int len);
};


#endif /* LEVELDB_LEVELDB_H_ */
