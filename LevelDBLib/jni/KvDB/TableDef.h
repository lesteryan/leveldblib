#ifndef _TABLE_DEF_H_
#define _TABLE_DEF_H_

#include "FieldType.h"
#include "SMDStream.h"
#include "SMDIndexSegment.h"
#include <string>
#include <vector>

namespace kvdbEngine
{
	class TableDef
	{
	public:
		TableDef(string name,bool IsSaveByGroup = true,SMD_INDEX_INDEXTYPE = SMD_INDEX_INDEXTYPE_BPTREE);
		~TableDef(void);

		//将表结构写入到流中
		bool write(SMDStream& stream);

		//从流中读取
		bool read(SMDStream& stream);

		//增加一个字段
		bool add(FieldType * field);

		//获取字段
		//@name 字段名称
		FieldType * getField(const std::string &name);

		//获取字段
		//@index 字段类型
		FieldType * getField(int index);


		//获取字段类型
		//@name 字段名称
		Enum_FieldType getFieldType(const std::string &name);

		//获取字段类型
		//@index 字段类型
		Enum_FieldType getFieldType(int index);

		//获取表名称
		string getTableName();

		//获取表索引类型
		SMD_INDEX_INDEXTYPE getIndexType();

		//获取存储属性
		bool getIsSaveByGroup()
		{
			return IsSaveByGroup;
		}
		//获取字段个数
		unsigned int getFieldCount()
		{
			return FieldSet.size();
		}
		
	protected:
		//设置表名称
		//@name 表名称
		bool setTableName(const string &name);
		bool IsOverFlow(int iIndex);
		void Release();
		void getIndexTypeFromShort(SMD_INDEX_INDEXTYPE& enumIndexType,short nIndexType);
	protected:
		string tableName;
		vector<FieldType *> FieldSet;
		bool IsSaveByGroup;
		SMD_INDEX_INDEXTYPE m_enumIndexType;
	};
}

#endif
