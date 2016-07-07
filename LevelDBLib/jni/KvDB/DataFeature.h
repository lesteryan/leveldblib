#ifndef _DATA_FEATURE_H_
#define _DATA_FEATURE_H_

#include "TableDef.h"
#include "Record.h"
#include "SMDStream.h"
#include "SMDPageFile.h"
#include "SMDPageManagement.h"
namespace kvdbEngine
{
	class DataFeature
	{
	public:
		DataFeature(TableDef *ptableStruct,SMDPageManagement *pPageManagement): type(SEG_TYPE_DATA)
		{
			setTableDef(ptableStruct);
			setPageManagement(pPageManagement);
			setPageFile(pPageManagement,type,ptableStruct);
		}
		virtual ~DataFeature(void);

		//增加一条记录
		int addRecord(Record* record);
		int addOneGroupRecords(vector<Record *> records);


		// 读取所有记录
		vector <Record *> readALL();
		vector<Record *> getOneGroupRecords( unsigned int offset );
		
		// 按照offset获取记录
		Record* getRecord(unsigned int offset);


	protected:

		//求出数字类型的相对值

		int getValueSize( long long value )
		{
			int size = 0;
			while(value != 0)
			{
				value /=10;
				size++;
			}
			return size;
		}

		//读取数据操作
		vector<Record * > readALLGroupByGroup();
		vector<Record * > readALLOneByOne();
		//释放Feature
		//设置pagefile
		bool setPageFile(SMDPageManagement *pPageManagement,SegType type,TableDef *ptableStruct)
		{
			this->pPageFile = pPageManagement->createPageFile(type,ptableStruct->getTableName(),0);
			if (pPageFile)
				return true;
			else	
				return false;
		}

		//设置pageManagement
		bool setPageManagement(SMDPageManagement * pPageManagement)
		{
			if (pPageManagement)
			{
				this->pPageManagement = pPageManagement;
				return true;
			}
			return false;
		}

		vector<Record *> getOGRecordsFromStream(SMDMemStream & stream);
		void releaseaddRecords(vector <Record *> records);
		
		//设置表结构
		bool setTableDef( TableDef * tStruct )
		{
			if (tStruct)
			{
				tableStruct = tStruct;
			}
			return false;
		}


	protected:
		TableDef * tableStruct;  //当前feture所指向的表结构
		SMDPageFile *pPageFile; // 写入文件对象
		SMDPageManagement *pPageManagement; // 当前所有文件管理对象
		const SegType type;
	};
}

#endif



