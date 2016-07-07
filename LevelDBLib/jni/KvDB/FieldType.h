#ifndef _FIELD_TYPE_H_
#define _FIELD_TYPE_H_

#include "SMDTableConfig.h"
#include "SMDStream.h"


using namespace std;

namespace kvdbEngine
{
	typedef union{
		double head_double;
		int head_int;
		struct {
			char * content;
			int nLength;
		}head_str;
	}head_t;
	
	typedef enum{
		//UNK
		HEAD_TYPE_UNKNOWN = 0,
		//double
		HEAD_TYPE_DOUBLE = 1,
		//int类型
		HEAD_TYPE_INT = 2,
		//string and binary
		HEAD_TYPE_STRING_BINARY = 3
	}Enum_HeadType;

	typedef struct _FieldHead{
		Enum_HeadType headtype; // 
		head_t headValue;

		_FieldHead()
		{
			headtype = HEAD_TYPE_UNKNOWN;
			headValue.head_double = 0;
		}
	}FieldHead;

	typedef enum
	{
		//UNK
		FIELD_TYPE_UNKNOWN = 0,
		//string 类型
		FIELD_TYPE_TEXT = 1,
		//varstring类型
		FIELD_TYPE_VAR_TEXT = 2,
		//binary类型
		FIELD_TYPE_BINARY = 3,
		//varbinary类型
		FIELD_TYPE_VAR_BINARY = 4,
		//float类型
		FIELD_TYPE_FLOAT = 5,
		//double类型
		FIELD_TYPE_DOUBLE = 6,
		//Fixed Length Double 类型
		FIELD_TYPE_FLDOUBLE = 7,
		//varint类型
		FIELD_TYPE_VAR_INT = 8,
		//short 类型
		FIELD_TYPE_SHORT = 9,
		//int 类型
		FIELD_TYPE_INT = 10,
		//long 类型
		FIELD_TYPE_LONG = 11
	}Enum_FieldType;

	class FieldType
	{
	public:
		virtual ~FieldType(void);

		//将字段结构写入到流中
		bool write(SMDStream& stream);

		//从字段结构中读取
		bool read(SMDStream& stream);

		//获取字段别名
		string getAlias();

		//设置字段别名
		//@alias 别名
		bool setAlias(string alias);

		//获取只读属性
		bool getIsReadOnly();

		//设置只读属性
		//@Is ReadOnly 只读属性
		bool setReadOnly(bool IsReadOnly);

		//获取只读属性
		bool getIsHaveHead();
		FieldHead * getFieldHead();
		//设置只读属性
		//@Is ReadOnly 只读属性
		bool setFieldHead(FieldHead & fhead);
		
		//获取字段类型
		Enum_FieldType getType();

		//获取字段名称
		string getName();

		//设置字段名称
		//@name  字段名称
		bool setName(string name);

		//获取字段是否可为空
		bool getIsAllowNULL();

		//设置字段是否为空属性
		//@IsNull 字段 true为空 false 不为空有默认值
		bool setALlowNULL( bool IsNull);
		
		//save 
		virtual bool save(SMDStream& stream);
		int size()
		{
			int n = 6;
			n += FieldName.size() + Alias.size();
			return n;
		}

	protected:
		FieldType();
		//设置字段类型
		//@typeCode 字段类型
		bool setFieldType(Enum_FieldType typeCode);

		bool writeFieldHead(SMDStream & stream);
		bool readFieldHead(SMDStream & stream);
		//默认初始化函数
		bool init(const string &name, Enum_FieldType Datatype,const string & Alias,
			bool IsHaveHead = false,bool ReadOnly = false, bool IsAllowNULL = true);
		//read子类的属性
		virtual bool readArr(SMDStream & stream);

		bool ReleaseHead()
		{
			if (IsHaveHead == true)
			{
				if (fhead.headtype == HEAD_TYPE_STRING_BINARY)
				{
					if (fhead.headValue.head_str.content != NULL)
					{
						free(fhead.headValue.head_str.content);
					}
				}
			}
			return true;
		}

		bool readExternFields(SMDStream& stream);
		bool writeExternFields(SMDStream& stream);
	protected:
		string Alias;
		bool ReadOnly;

		Enum_FieldType DataType;
		string FieldName;
		bool IsAllowNULL;
		bool IsHaveHead;
		FieldHead fhead;

		std::map<std::string,std::string> externFields;
		//bool IsRelative; //求出相对值
	};
}

#endif
