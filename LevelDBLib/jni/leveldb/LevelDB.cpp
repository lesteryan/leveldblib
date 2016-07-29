#include "LevelDB.h"

LevelDB::LevelDB():db(NULL)
{
}

LevelDB::~LevelDB()
{
	close();
}

Status LevelDB::open(const std::string& _dbPath, const std::string& _dbName, bool _readOnly, bool _writeOnly)
{
	option.create_if_missing = _readOnly ? false : true;
	option.read_only = _readOnly;
	option.write_only = _writeOnly;

	Status statue = DB::Open(option, _dbPath, _dbName, &db);

	if(statue.ok())
	{
		dbPath = _dbPath;
		dbName = _dbName;
	}

	return statue;
}

Status LevelDB::insert(const Slice& key, const Slice& value)
{
	if(db == NULL)
		return Status::IOError("database is not open");
	else if(key.size() != KEY_LENGTH)
		return Status::InvalidArgument("key length invalid");

	return db->Put(WriteOptions(), key, value);
}

Status LevelDB::atomReady()
{
	if(db == NULL)
		return Status::IOError("database is not open");

	batch.Clear();

	return Status::OK();
}

Status LevelDB::atomInsert(const Slice& key, const Slice& value)
{
	if(db == NULL)
		return Status::IOError("database is not open");
	else if(key.size() != KEY_LENGTH)
		return Status::InvalidArgument("key length invalid");

	db->Write(WriteOptions(), &batch);
	batch.Put(key, value);

	return Status::OK();
}

Status LevelDB::atomCommit()
{
	if(db == NULL)
		return Status::IOError("database is not open");

	return db->Write(WriteOptions(), &batch);
}

Status LevelDB::query(const Slice& key, std::string& value)
{
	if(db == NULL)
		return Status::IOError("database is not open");

	return db->Get(ReadOptions(), key, &value);
}

void LevelDB::close()
{
	if(db != NULL)
	{
		db->~DB();
		db = NULL;
	}
}

Status LevelDB::destory()
{
	if(db == NULL)
		return Status::IOError("database is not open");

	batch.Clear();

	return DestroyDB(dbPath, dbName, option);
}

void LevelDB::makeKey(unsigned short type, unsigned long long key, Slice& slice)
{
	unsigned char *newKey = (unsigned char *)malloc(10);

	integer2array(type, newKey, 2);
	integer2array(key, newKey + 2, 8);

	slice = Slice((const char *)newKey, 10);
}

void LevelDB::integer2array(unsigned long long value, unsigned char * dest, int len)
{
	if(dest == NULL)
		return ;

    for(int i = 0 ; i < len ; i++)
    {
        dest[i] = (unsigned char)(((value >> (len - i - 1) * 8)) & 0xff);
    }
}

#ifdef BUILD_HADOOP
Status LevelDB::getVirtualFiles(std::vector<VirtualMemFile *>& _files)
{
	_files.clear();

	if(db == NULL)
	{
		return Status::IOError("database is not open");
	}

	_files = db->getFiles();

	return Status::OK();
}
#endif



//	bool open(const char * _dbname, bool read_only, bool write_only)
//	{
////		myOptions.create_if_missing = true;
//////		myOptions.reuse_logs = true;
////		myOptions.read_only = read_only;
////		myOptions.write_only = write_only;
////		dbpath = "/sdcard/tmp/leveldb/";
////		dbname = std::string(_dbname);
////
////		if(kvdb != NULL)
////			delete kvdb;
////		kvdb = new KvDB();
////		if(kvdb->open() == false)
////			return false;
////
////		status = leveldb::DB::Open(myOptions, dbpath, dbname, &db);
////
////		LOGI(status.ok() ? "leveldb open success" : "leveldb open failed");
////		if(status.ok() == false)
////			LOGE(status.ToString().data());
//
//		return true;
//	}
//
//	jlong insert()
//	{
////		int count;
////		int index;
////		string key;
////		string record;
////		leveldb::WriteBatch batch;
////		count = 0;
////
////		timeUtil.start() ;
//////		LOGE("ready write ti batch");
//////		while((index = kvdb->getRecord(record)) != -1)
//////		{
//////			count++;
//////			Int2String(index, key);
//////			LOGE("temp1");
//////			batch.Put(key, record);
//////			LOGE("temp2");
//////			if(count % 100 ==0)
//////			{
//////				sprintf(tmpArray, "times = %d", count);
//////				LOGE(tmpArray);
//////
////////				status = db->Write(leveldb::WriteOptions(), &batch);
//////
////////				if(!status.ok())
////////				{
////////					LOGE(status.ToString().data());
////////					return -1;
////////				}
////////				batch.Clear();
//////			}
//////		}
////
////		batch.Put("SOGOU__", "NAVI__");
////
//////		LOGE("write to batch finished");
////
////		leveldb::WriteOptions opt= leveldb::WriteOptions();
////		opt.sync = true;
////		status = db->Write(leveldb::WriteOptions(), &batch);
////
////		if(!status.ok())
////		{
////			LOGE("insert failed");
////			LOGE(status.ToString().data());
////			return -1;
////		}
////
////		timeUtil.stop();
////
//////		sprintf(tmpArray, "insert times = %d", count);
//////		LOGE(tmpArray);
////
////		LOGI("leveldb insert success");
//
//		return 1;
//	}
//
//	bool clean()
//	{
////		db->NewIterator()
//		return false;
//	}
//
//	jlong query()
//	{
//		int count = TIMES;
//		int index;
//		string key, value;
//		timeUtil.start();
////		while(count-- >= 0)
////		{
////			index = kvdb->getRandomIndex();
////			Int2String(index, key);
////			string leveldbRecord, kvdbrecord;
////			status = db->Get(leveldb::ReadOptions(), key, &leveldbRecord);
////			if(!status.ok())
////				return -1;
//////
//////			if(!kvdb.getRecord(index, kvdbrecord))
//////				LOGE("get record failed");
//////
//////			if(leveldbRecord.compare(kvdbrecord) != 0)
//////			{
////////				sprintf(tmpArray, "leveldb %s size = %d navilinkdb %s size = %d", str2hex(leveldbRecord), leveldbRecord.size(), str2hex(kvdbrecord), kvdbrecord.size());
////////				LOGE(tmpArray);
//////				LOGE("compare error");
//////
//////				break;
//////			}
////
////		}
////
////		status = db->Get(leveldb::ReadOptions(), "SOGOU__", &value);
////		if(status.ok())
////		{
////			LOGI((std::string("query success ") + value).data());
////		}
////		else
////		{
////			LOGE("query failed");
////			LOGE(status.ToString().data());
////		}
////
////		timeUtil.stop();
//
//		return 1;
//	}
//
//	bool close()
//	{
//		LOGI("ready close db");
//
//		if(kvdb != NULL)
//		{
//			delete kvdb;
//			kvdb = NULL;
//		}
//
//		if(db != NULL)
//		{
//			db->~DB();
////			delete [] db;
//			db = NULL;
//		}
//
//		LOGI("leveldb closed success");
//
//		return true;
//	}
//
//	bool reopen()
//	{
//
//		return false;
//	}
//
////	void test()
////	{
////		KvDB kvdb;
////		leveldb::DB* db;
////		leveldb::Options myOptions;
////		myOptions.create_if_missing = true;
////		string dbpath = "/sdcard/tmp/test";
////		leveldb::Status status = leveldb::DB::Open(myOptions, dbpath, &db);
////		assert(status.ok());
////		string key1 = "sogou";
////		string key2 = "map";
////		LOGE("Open db OK");
////
////		string record;
////		char buffer[255];
////		int len;
////		int count = TIMES;
////		int ret;
////
////		leveldb::WriteBatch batch;
////		string key;
////		int index;
////
////		timeUtil.start();
////		while(count-- >= 0)
////		{
////			index = kvdb.getRecord(record);
////			if(index == -1)
////			{
////				count++;
////				continue;
////			}
////
////			Int2String(index, key);
////			batch.Put(key, record);
////		}
////
////		leveldb::Status state = db->Write(leveldb::WriteOptions(), &batch);
////
////		if(!state.ok())
////			LOGE("write error");
////		else
////			LOGE("write ok");
////
////		timeUtil.stop();
////
////		sprintf(buffer, "add %d times used time %ld", TIMES - count, timeUtil.getTime());
////		LOGE(buffer);
////
////		delete db;
////		status = leveldb::DB::Open(myOptions, dbpath, &db);
////				assert(status.ok());
////				LOGE("Open db OK");
////
////		count = TIMES;
////		string tempRecord;
////		timeUtil.start();
////		while(count-- >= 0)
////		{
////			index = kvdb.getRandomIndex();
////			Int2String(index, key);
////			string leveldbRecord, kvdbrecord;
////			state = db->Get(leveldb::ReadOptions(), key, &leveldbRecord);
////			if(!state.ok())
////				LOGE("query failed");
//////
//////			if(!kvdb.getRecord(index, kvdbrecord))
//////				LOGE("get record failed");
//////
//////			if(leveldbRecord.compare(kvdbrecord) != 0)
//////			{
////////				sprintf(tmpArray, "leveldb %s size = %d navilinkdb %s size = %d", str2hex(leveldbRecord), leveldbRecord.size(), str2hex(kvdbrecord), kvdbrecord.size());
////////				LOGE(tmpArray);
//////				LOGE("compare error");
//////
//////				break;
//////			}
////
////		}
////
////		timeUtil.stop();
////
////		sprintf(tmpArray,"query %d times cost %ld ms", TIMES , timeUtil.getTime());
////		LOGE(tmpArray);
////
////	}
