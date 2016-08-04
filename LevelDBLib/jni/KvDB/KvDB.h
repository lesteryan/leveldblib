#ifndef KVDB_KVDB_H_
#define KVDB_KVDB_H_

#include "SMDSStorage.h"
#include "Record.h"
#include "TableDef.h"
#include <vector>
#include <stdio.h>
#include <util/LogUtil.h>

class KvDB
{
	public:
	KvDB()
	{
		isOpenSuccess = false;
		fp = NULL;
	}

	bool open(const char *dbPath, const char * indexPath)
	{
		if(table.open(dbPath))
			LOGI("open smd table success");
		else
		{
			LOGE("open smd table failed");
			return false;
		}

		int count;
		fp = fopen(indexPath, "r");
		if(fp != 0)
		{
			LOGI("open index file success");
		}
		else
		{
			LOGE("open index file failed");
			return false;
		}

		isOpenSuccess = true;

		return true;
	}

	//get a record and index
	int getRecord(string &data)
	{
		if(isOpenSuccess == false)
		{
			return 0;
		}
		data.clear();

		int index;

		if(fscanf(fp, "%d", &index) <= 0)
		{
			LOGW("read end");
			return false;
		}

//		LOGI("index = %d", index);

		kvdbEngine::Record *record = table.search("NaviData", index);

		if(record == 0)
		{
			sprintf(tempBuffer, "get index failed, index = %d", index);
			LOGE(tempBuffer);
			return 0;
		}
		string temp;
		for(int i = 0 ; i < 4 ; i++)
		{
			if(record->getFieldAsString(i ,temp))
				data+=temp;
		}

		return index;
	}

	//get record by index
	bool getRecord(const int index, string &data)
	{
		if(isOpenSuccess == false)
			return false;

		data.clear();

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

		return index;
	}

	~KvDB()
	{
		LOGW("kvdb deleted");
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
};

#endif
