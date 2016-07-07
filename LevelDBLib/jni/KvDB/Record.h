#ifndef _RECORD_H_
#define _RECORD_H_

#include "TableDef.h"
#include "SMDStream.h"
#include "NaviLock.h"
#include "NaviGuard.h"

#include <vector>

namespace kvdbEngine
{
	typedef union {
		short       Short;
		int         Integer;
		long long		Long;
		float       Float;
		double      Double;

		/* 定小数位的数; */
		struct {
			int     nLength;
			long long    FLDouble;
		} FixedLengthDouble;

		//定长与变长字符串和二进制
		struct {
			int     nLength;
			char    *buffer;
		} TextAndBinary;

		//变长整型
		struct {
			int     nCount;
			unsigned char data[5];
		} VarInteger;

	} Field_Store;

	typedef struct _Field_t
	{
		Enum_FieldType fieldType;  //字段类型
		Field_Store    fieldValue; //字段存储的数据
		int            nFieldSize; //字段所占空间
	}Field;

	class Record 
	{
	public:
		Record(TableDef * tableStruct);
		~Record(void);
		void Release();

		//写入一整条记录
		bool write(SMDStream& stream);
		bool read(SMDStream& stream);

	protected:
		//从流中读取VarInt
		bool readVarInt(SMDStream& stream,int iIndex);

	public:

		//求相对值
		Record * subRecord(Record * p);
		void addRecord(Record * p);
		//返回记录中的Field
		//@iIndex  索引号
		Field * getField(int iIndex);

		//写入字段值
		bool setFieldShort(int iIndex,short value); //short
		bool setFieldInteger(int iIndex,int value, bool flag = true);  //int
		bool setFieldLong(int iIndex,long long value, bool flag = true);  //long

		bool setFieldFloat(int iIndex,float value); //float
		bool setFieldDouble(int iIndex,double value);//double
		bool setFieldFLDouble(int iIndex,double value); //FLDouble

		bool setFieldBinary(int iIndex,char * buffer,int size);//定长变长二进制
		bool setFieldString(int iIndex,const string & str); // 定长变长字符串

		//获取字段值
		bool getFieldAsShort(int iIndex, short &sval);
		bool getFieldAsInteger( int iIndex, int & ival ); //int与varint都包含
		
		bool getFieldAsLong(int iIndex, long long & lval);

		bool getFieldAsFloat(int iIndex, float &fval);
		bool getFieldAsDouble( int iIndex, double &dval); //普通的Double与定长的double

		bool getFieldAsString( int iIndex,string & buffer );// 定长变长字符串
		bool getFieldAsBinary(int iIndex,char *& buffer,int &size);//定长变长二进制

		//初始化字段类型数组
		bool initFields(TableDef * tableStruct);

		//标志位函数

		//计算所需空间 
		//@num 需要多少位字节
		unsigned int calcSize(unsigned int num);


		//设置bit位置为1，这里不进行越界提示，请使用时自行加入
		//@bit 位置标号
		void setBit(unsigned int bit);

		//读取bit位置的值，返回值为1、0,这里不进行越界提示，请使用时自行加入
		//@bit 位置标号
		int readBit(unsigned int bit);

		//设置bit为0
		//@bit 位置标号
		void ClearBit(unsigned int bit);

		//初始化标志位
		void initFlagBit(TableDef * tableStruct);


		//VarInteger的值
		//@field为VarInt类型的字段
		int getVarInt(int iIndex);

	protected:
		//初始化bit
		//@size 字节数
		void initBit(int size);

		//判断索引是否越界
		//@iIndex 为字段索引
		bool IsOverFlow(int iIndex);

		int getValueSize(long long value);
	private:
		unsigned int Zig(int value);
		int Zag(unsigned int ziggedValue);
		unsigned char* varIntWrite(int value, unsigned char* data_byte,int & count);
        
        NaviLock m_lock;

	protected:
		unsigned int size; //标记字段占用空间大小记录占用空间大小

		char *fieldFlag; //字段存储标记

		vector<Field> Fields;

		TableDef * tableStruct; //所属表结构
	};
}
#endif 

