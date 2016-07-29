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
	ret = kvdb.open("/sdcard/beijing.navi", "sdcard/naviindex.txt");
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

	char temp[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a};
//	Slice key("AAAAAAAAAA", 10);
	Slice key;

	LevelDB::makeKey(0x1122, 0x33445566778899aa, key);

	statue = leveldb.insert(key, "Sogou");
	if(statue.ok())
		LOGI("leveldb insert success");
	else
	{
		LOGE("leveldb insert failed, %s", statue.ToString().data());
		return ;
	}
	leveldb.close();

	statue = leveldb.open("/sdcard/tmp/leveldb/", "beijing", false, true);
	if(statue.ok())
		LOGI("leveldb open success");
	else
	{
		LOGE("leveldb open failed");
		return ;
	}

	std::string value;
//	LevelDB::makeKey(0x1122, 0x33445566778899aa, key);
	statue = leveldb.query(key, value);
	if(statue.ok())
		LOGI("leveldb query success, %s", value.data());
	else
	{
		LOGE("leveldb query failed, %s", statue.ToString().data());
//		return ;
	}

//	std::vector<VirtualMemFile *> files;
//	leveldb.getVirtualFiles(files);
//	for(int i = 0 ; i< files.size() ; i++)
//		LOGI(files[i]->toString().data());
	leveldb.close();

//	kvdb.getRecord()
}
