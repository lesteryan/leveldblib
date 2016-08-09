 #include <jni.h>
#include "LevelDB.h"
#include "KvDB/KvDB.h"
//LevelDB *levelDB = NULL;
//
//extern "C" bool Java_com_sogou_leveldblib_LevelDB_open(JNIEnv* env, jobject thiz, jstring dbname, bool read_only, bool write_only)
//{
//	jboolean iscopy;
//	if(levelDB == NULL)
//		levelDB = new LevelDB();
//
//	;
//	return levelDB->open(env->GetStringUTFChars(dbname, &iscopy), read_only, write_only);
//}
//
//extern "C" jlong Java_com_sogou_leveldblib_LevelDB_insert(JNIEnv* env, jobject thiz)
//{
//	if(levelDB == NULL)
//		return -1;
//
//	return levelDB->insert();
//}
//
//extern "C" bool Java_com_sogou_leveldblib_LevelDB_clean(JNIEnv* env, jobject thiz)
//{
//	if(levelDB == NULL)
//		return -1;
//
//	return levelDB->clean();
//}
//
//extern "C" jlong Java_com_sogou_leveldblib_LevelDB_query(JNIEnv* env, jobject thiz)
//{
//	if(levelDB == NULL)
//		return -1;
//
//	return levelDB->query();
//}
//
//
//extern "C" bool Java_com_sogou_leveldblib_LevelDB_close(JNIEnv* env, jobject thiz)
//{
//	levelDB->close();
//	delete levelDB;
//	levelDB = NULL;
//	return true;
//}


extern "C" void Java_com_sogou_leveldblib_LevelDB_test(JNIEnv* env, jobject thiz)
{
	bool ret;
	Status statue;

	KvDB kvdb;
	ret = kvdb.open("/sdcard/beijing/beijing.navi", "sdcard/beijing/beijing.index");
	if(ret)
		LOGI("kvdb open success");
	else
	{
		LOGE("kvdb open failed, %s", statue.ToString().data());
		return ;
	}

	LevelDB leveldb;
	statue = leveldb.open("/sdcard/tmp/leveldb/", "beijing", false, true);
	if(statue.ok())
		LOGI("leveldb open success");
	else
	{
		LOGE("leveldb open failed, %s", statue.ToString().data());
		return ;
	}

	int index;
	string data[] = {"Hello", "World", "NaviLink", "Sogou", "beijing", "Good"};

	leveldb.atomReady();
	for(int i = 0 ; i < sizeof(data)/sizeof(std::string) ; i++)
	{
		statue = leveldb.insert(LevelDB::KEY_HEAD_ROADNAME, i, data[i]);
		if(!statue.ok())
		{
			LOGE("leveldb insert failed, %s", statue.ToString().data());
			return ;
		}
	}
	leveldb.atomCommit();

//	leveldb.atomReady();
//	while((index = kvdb.getRecord(data)) != 0)
//	{
//		statue = leveldb.insert(0x0001, index, data);
//		if(!statue.ok())
//		{
//			LOGE("leveldb insert failed, %s", statue.ToString().data());
//			return ;
//		}
//	}
//	leveldb.atomCommit();

	LOGE("leveldb insert %s", statue.ToString().data());


	leveldb.close();

	statue = leveldb.make();
	if(statue.ok() == false)
	{
		LOGI("leveldb make error");
		LOGI(statue.ToString().data());
	}
	else
	{
		LOGI("leveldb make success");
	}

	statue = leveldb.open("/sdcard/tmp/leveldb/", "beijing", true, false);
	if(statue.ok())
		LOGI("leveldb open success");
	else
	{
		LOGE("leveldb open failed %s", statue.ToString().data());
		return ;
	}


	string queryResult;
	for(int i = 0 ; i < sizeof(data)/sizeof(std::string) ; i++)
	{
		statue = leveldb.query(LevelDB::KEY_HEAD_ROADNAME, i, queryResult);
		if(!statue.ok())
		{
			LOGE("leveldb insert failed, %s", statue.ToString().data());
			return ;
		}
		LOGE("query result %s", queryResult.data());
	}

//	std::string value;
////	LevelDB::makeKey(0x1122, 0x33445566778899aa, key);
//	statue = leveldb.query(key, value);
//	if(statue.ok())
//		LOGI("leveldb query success, %s", value.data());
//	else
//	{
//		LOGE("leveldb query failed, %s", statue.ToString().data());
////		return ;
//	}
//
////	std::vector<VirtualMemFile *> files;
////	leveldb.getVirtualFiles(files);
////	for(int i = 0 ; i< files.size() ; i++)
////		LOGI(files[i]->toString().data());
	leveldb.close();

//	kvdb.getRecord()
}
