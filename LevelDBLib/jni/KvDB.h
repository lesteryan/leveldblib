#ifndef KVDB_H_
#define KVDB_H_

#include "SMDSStorage.h"
#include "Record.h"
#include "TableDef.h"
#include "LogUtil.h"
#include <vector>
#include <stdio.h>


class KvDB
{
	public:
	KvDB()
	{
		isOpenSuccess = false;
		fp = NULL;
	}

	bool open()
	{
		if(table.open("sdcard/beijing.navi"))
			LOGE("open smd table success");
		else
		{
			LOGE("open smd table failed");
			return false;
		}

		int count;
		fp = fopen("sdcard/naviindex.txt", "r");
		if(fp != 0)
		{
			LOGE("open index file success");
		}
		else
		{
			LOGE("open index file failed");
			return false;
		}

		isOpenSuccess = true;

		return true;
	}

	//get a record, return index
	int getRecord(string &data)
	{
		LOGE("temp01");
		if(isOpenSuccess == false)
		{
			return -1;
		}
		data.clear();
		LOGE("temp02");
		int index;
		if(fscanf(fp, "%d", &index) <= 0)
		{
			LOGE("read end");
			return -1;
		}
		LOGE("temp03");
//		sprintf(tempBuffer, "get reocrd success, index = %d", index);
//		LOGE(tempBuffer);


		kvdbEngine::Record *record = table.search("NaviData", index);
		LOGE("temp031");
		if(record == 0)
		{
			LOGE("temp032");
			sprintf(tempBuffer, "get index failed, index = %d", index);
			LOGE(tempBuffer);
			return -1;
		}LOGE("temp04");
		string temp;
		for(int i = 0 ; i < 4 ; i++)
		{
			if(record->getFieldAsString(i ,temp))
				data+=temp;
		}
		LOGE("temp05");
//		sprintf(tempBuffer, "get reocrd success, index = %d ,size = %d", index, data.size());
//		LOGE(tempBuffer);

		readedIndex.push_back(index);
		return index;
	}

	//get record by index
	bool getRecord(int index, string &data)
	{
		if(isOpenSuccess == false)
			return false;

		data.clear();

//		int index;
//		fscanf(fp, "%d", &index);

//		sprintf(temp, "get index success, index = %d", index);
//		LOGE(temp);


		kvdbEngine::Record *record = table.search("NaviData", index);
		if(record == 0)
		{
			sprintf(tempBuffer, "get index failed, index = %d", index);
			LOGE(tempBuffer);
			return false;
		}
		string temp;
		for(int i = 0 ; i < 4 ; i++)
		{
			if(record->getFieldAsString(i ,temp))
				data.append(temp);
		}

//		readedIndex.push_back(index);
		return index;
	}

	int getRandomIndex()	//随机获得一个曾经read过的index
	{
		if(readedIndex.size() != 0)

			return readedIndex[random() % readedIndex.size()];

		int index;

		if(fscanf(fp, "%d", &index) <= 0)
		{
			LOGE("read end");
			return -1;
		}

		return index;
	}

	vector<int> getAllReadedIndex()
	{
		return readedIndex;
	}

	void setReadedIndex(vector<int> readedIndex)
	{
		this->readedIndex = readedIndex;
	}

	~KvDB()
	{
		LOGE("kvdb deleted");
		if(isOpenSuccess)
		{
			table.close();
		}

		if(fp != NULL)
			fclose(fp);
	}
private:
	kvdbEngine::SMDTable table;
	FILE *fp;
	bool isOpenSuccess;
	char tempBuffer[255];
	vector<int> readedIndex;

};

#endif
