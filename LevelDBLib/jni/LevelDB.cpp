#include <jni.h>
#include "leveldb/db.h"
#include "leveldb/write_batch.h"
#include "LogUtil.h"
#include "TimeUtil.h"
#include "KvDB.h"

#define TIMES 10000

class LevelDB
{
public:
	LevelDB()
	{
	    kvdb = NULL;
	}

	bool open()
	{
		myOptions.create_if_missing = true;
		dbpath = "/sdcard/tmp/leveldb";
		dbname = "navi";

		if(kvdb != NULL)
			delete kvdb;
		kvdb = new KvDB();
		if(kvdb->open() == false)
			return false;

		status = leveldb::DB::Open(myOptions, dbpath, dbpath, &db);

		LOGE(status.ok() ? "leveldb open success" : "leveldb open failed");
		if(status.ok() == false)
			LOGE(status.ToString().data());

		return status.ok();
	}

	jlong insert()
	{
		int count;
		int index;
		string key;
		string record;
		leveldb::WriteBatch batch;
		count = 0;

		timeUtil.start();
		while((index = kvdb->getRecord(record)) != -1)
		{
			count++;
			Int2String(index, key);
			batch.Put(key, record);

			if(count % 100 ==0)
			{
				sprintf(tmpArray, "times = %d", count);
				LOGE(tmpArray);

//				status = db->Write(leveldb::WriteOptions(), &batch);

//				if(!status.ok())
//				{
//					LOGE(status.ToString().data());
//					return -1;
//				}
//				batch.Clear();
			}
		}

		LOGE("write to batch finished");

		leveldb::WriteOptions opt= leveldb::WriteOptions();
		status = db->Write(leveldb::WriteOptions(), &batch);

		if(!status.ok())
		{
			LOGE(status.ToString().data());
			return -1;
		}

		timeUtil.stop();

		sprintf(tmpArray, "insert times = %d", count);
		LOGE(tmpArray);

		LOGE("leveldb insert success");

		return timeUtil.getTime();
	}

	bool clean()
	{
//		db->NewIterator()
		return false;
	}

	jlong query()
	{
		int count = TIMES;
		int index;
		string key;
		timeUtil.start();
		while(count-- >= 0)
		{
			index = kvdb->getRandomIndex();
			Int2String(index, key);
			string leveldbRecord, kvdbrecord;
			status = db->Get(leveldb::ReadOptions(), key, &leveldbRecord);
			if(!status.ok())
				return -1;
//
//			if(!kvdb.getRecord(index, kvdbrecord))
//				LOGE("get record failed");
//
//			if(leveldbRecord.compare(kvdbrecord) != 0)
//			{
////				sprintf(tmpArray, "leveldb %s size = %d navilinkdb %s size = %d", str2hex(leveldbRecord), leveldbRecord.size(), str2hex(kvdbrecord), kvdbrecord.size());
////				LOGE(tmpArray);
//				LOGE("compare error");
//
//				break;
//			}

		}

		timeUtil.stop();

		return timeUtil.getTime();
	}

	bool close()
	{
		delete kvdb;
		kvdb = NULL;
		delete db;
		db = NULL;

		return true;
	}

	bool reopen()
	{
		vector<int> readedIndex = kvdb->getAllReadedIndex();

		if(!close())
		{
			LOGE("close error");
			return false;
		}

		LOGE("close success");

		if(!open())
		{
			LOGE("open error");
			return false;
		}

		LOGE("open success");

		kvdb->setReadedIndex(readedIndex);

		return true;
	}

//	void test()
//	{
//		KvDB kvdb;
//		leveldb::DB* db;
//		leveldb::Options myOptions;
//		myOptions.create_if_missing = true;
//		string dbpath = "/sdcard/tmp/test";
//		leveldb::Status status = leveldb::DB::Open(myOptions, dbpath, &db);
//		assert(status.ok());
//		string key1 = "sogou";
//		string key2 = "map";
//		LOGE("Open db OK");
//
//		string record;
//		char buffer[255];
//		int len;
//		int count = TIMES;
//		int ret;
//
//		leveldb::WriteBatch batch;
//		string key;
//		int index;
//
//		timeUtil.start();
//		while(count-- >= 0)
//		{
//			index = kvdb.getRecord(record);
//			if(index == -1)
//			{
//				count++;
//				continue;
//			}
//
//			Int2String(index, key);
//			batch.Put(key, record);
//		}
//
//		leveldb::Status state = db->Write(leveldb::WriteOptions(), &batch);
//
//		if(!state.ok())
//			LOGE("write error");
//		else
//			LOGE("write ok");
//
//		timeUtil.stop();
//
//		sprintf(buffer, "add %d times used time %ld", TIMES - count, timeUtil.getTime());
//		LOGE(buffer);
//
//		delete db;
//		status = leveldb::DB::Open(myOptions, dbpath, &db);
//				assert(status.ok());
//				LOGE("Open db OK");
//
//		count = TIMES;
//		string tempRecord;
//		timeUtil.start();
//		while(count-- >= 0)
//		{
//			index = kvdb.getRandomIndex();
//			Int2String(index, key);
//			string leveldbRecord, kvdbrecord;
//			state = db->Get(leveldb::ReadOptions(), key, &leveldbRecord);
//			if(!state.ok())
//				LOGE("query failed");
////
////			if(!kvdb.getRecord(index, kvdbrecord))
////				LOGE("get record failed");
////
////			if(leveldbRecord.compare(kvdbrecord) != 0)
////			{
//////				sprintf(tmpArray, "leveldb %s size = %d navilinkdb %s size = %d", str2hex(leveldbRecord), leveldbRecord.size(), str2hex(kvdbrecord), kvdbrecord.size());
//////				LOGE(tmpArray);
////				LOGE("compare error");
////
////				break;
////			}
//
//		}
//
//		timeUtil.stop();
//
//		sprintf(tmpArray,"query %d times cost %ld ms", TIMES , timeUtil.getTime());
//		LOGE(tmpArray);
//
//	}

private:
	KvDB *kvdb;

	leveldb::DB* db;
	leveldb::Options myOptions;
	leveldb::Status status;
	string dbpath, dbname;

	TimeUtil timeUtil;
	char tmpArray[512];
//	int len;

	 void Int2String(int intValue, string & Integer)
	 {
		char buf[4];
		buf[0] = (char) (intValue >> 24);
		buf[1] = (char) (intValue >> 16);
		buf[2] = (char) (intValue >> 8);
		buf[3] = (char) intValue;
		Integer = string(buf, 4);
	}
};

static LevelDB *levelDB = NULL;

extern "C" bool Java_com_sogou_leveldblib_LevelDB_open(JNIEnv* env, jobject thiz)
{
	if(levelDB != NULL)
		delete levelDB;

	levelDB = new LevelDB();

	return levelDB->open();

}

extern "C" jlong Java_com_sogou_leveldblib_LevelDB_insert(JNIEnv* env, jobject thiz)
{
	if(levelDB == NULL)
		return -1;

	return levelDB->insert();
}

extern "C" bool Java_com_sogou_leveldblib_LevelDB_clean(JNIEnv* env, jobject thiz)
{
	if(levelDB == NULL)
		return -1;

	return levelDB->clean();
}

extern "C" jlong Java_com_sogou_leveldblib_LevelDB_query(JNIEnv* env, jobject thiz)
{
	if(levelDB == NULL)
		return -1;

	return levelDB->query();
}


extern "C" bool Java_com_sogou_leveldblib_LevelDB_close(JNIEnv* env, jobject thiz)
{
	delete levelDB;
	levelDB = NULL;
	return true;
}

extern "C" bool Java_com_sogou_leveldblib_LevelDB_reopen(JNIEnv* env, jobject thiz)
{
	levelDB = new LevelDB();

	return levelDB->reopen();
}
