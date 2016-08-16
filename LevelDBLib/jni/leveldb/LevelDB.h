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
#include "Reference.h"
#ifdef BUILD_HADOOP
#include "util/fileSystem/VirtualMemFile.h"
#endif

using namespace leveldb_navi;

class LevelDB
{
public:
    static const char KEY_LENGTH = (sizeof(short) + 4);

	static const short KEY_HEAD_NAVILINK = 0x0001;  //初步设计所有key均为6字节，(2字节类型+4字节id)
    static const short KEY_HEAD_ROADNAME = 0x0002;
    
    LevelDB();
    ~LevelDB();
    
    Status open(const std::string& _dbPath, const std::string& _dbName, bool _readOnly, bool _writeOnly);
    Status open(const std::string& _dbPath);
    void close();
    
    bool comparePath(const std::string& _dbPath, const std::string& _dbName);
    bool isOpen();
    Reference reference;

	Status insert(const Slice& key, const Slice& value);    //leveldb原装版insert
    Status insert(const unsigned short type, const unsigned int id, const Slice& value);    //leveldb_navi新版insert

	Status atomReady(); //启动原子写方法，启动后执行insert将会写入内存而不是文件
	Status atomCommit();    //提交原子写方法，将内存中的数据提交到文件中并清空内存

	Status query(const Slice& key, std::string& value); //leveldb原装版query
    Status query(const unsigned short type, const unsigned int id, std::string& value); //leiveldb_navi新版insert

	Status destory();   //删除数据库方法

	Status make();  //压缩leveldb数据，为尽量减少对leveldb的修改和对leveldb整体的破坏，没有将这部分单独提出来

#ifdef BUILD_HADOOP
	Status getVirtualFiles(std::vector<VirtualMemFile *>& _files);  //为hadoop序列化提供，获得虚拟文件系统下所有文件
#endif

private:
	DB* db;
	WriteBatch batch;   //原子写写入目标
	Options option;
    

	std::string dbPath;
	std::string dbName;

    bool isAtomic;  //是否已启动原子写标志

    void integer2array(const long long value, char * dest, const int len);
    void makeKey(const short type, const int id, char * key);
    
  
};


#endif /* LEVELDB_LEVELDB_H_ */
