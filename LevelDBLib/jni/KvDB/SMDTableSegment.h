#ifndef _SMD_TABLE_SEGMENT_H_
#define _SMD_TABLE_SEGMENT_H_

#include <string>
#include <vector>
using namespace std;

#include "TableDef.h"
#include "SMDSegment.h"
#include "SMDPageManagement.h"
#include "SMDPageFile.h"

namespace kvdbEngine
{
	class SMDTableSegment :
		public SMDSegment
	{
	public:
		SMDTableSegment(SMDPageManagement* pageMgr);
		~SMDTableSegment(void);

		// 获取内容段类型
		virtual SegType getType()
		{
			return SEG_TYPE_TABLES;
		}

		// 序列化到内存中
		virtual bool save();

		// 从文件读取到内存
		// stroage在读取数据结构时会调用此函数
		virtual bool read();

		//增加一个表结构,不负责释放def
		bool addTableDef(TableDef* def);


		//获取一个表结构，外部可以修改，但不用释放，由SMDTableSegment负责释放
		TableDef* getTableDef(int index);
		TableDef* getTableDef(const std::string &tableName);

		//获取第index个表的名称
		bool getTableName(int index, string & tbName);

		int TableSum()
		{
			return (int)tableSet.size();
		}

	protected:
		void Release();
	protected:
		vector<TableDef *> tableSet;
		SMDPageManagement* pageMgr;
	};
}
#endif

