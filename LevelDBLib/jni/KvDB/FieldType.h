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
		//int����
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
		//string ����
		FIELD_TYPE_TEXT = 1,
		//varstring����
		FIELD_TYPE_VAR_TEXT = 2,
		//binary����
		FIELD_TYPE_BINARY = 3,
		//varbinary����
		FIELD_TYPE_VAR_BINARY = 4,
		//float����
		FIELD_TYPE_FLOAT = 5,
		//double����
		FIELD_TYPE_DOUBLE = 6,
		//Fixed Length Double ����
		FIELD_TYPE_FLDOUBLE = 7,
		//varint����
		FIELD_TYPE_VAR_INT = 8,
		//short ����
		FIELD_TYPE_SHORT = 9,
		//int ����
		FIELD_TYPE_INT = 10,
		//long ����
		FIELD_TYPE_LONG = 11
	}Enum_FieldType;

	class FieldType
	{
	public:
		virtual ~FieldType(void);

		//���ֶνṹд�뵽����
		bool write(SMDStream& stream);

		//���ֶνṹ�ж�ȡ
		bool read(SMDStream& stream);

		//��ȡ�ֶα���
		string getAlias();

		//�����ֶα���
		//@alias ����
		bool setAlias(string alias);

		//��ȡֻ������
		bool getIsReadOnly();

		//����ֻ������
		//@Is ReadOnly ֻ������
		bool setReadOnly(bool IsReadOnly);

		//��ȡֻ������
		bool getIsHaveHead();
		FieldHead * getFieldHead();
		//����ֻ������
		//@Is ReadOnly ֻ������
		bool setFieldHead(FieldHead & fhead);
		
		//��ȡ�ֶ�����
		Enum_FieldType getType();

		//��ȡ�ֶ�����
		string getName();

		//�����ֶ�����
		//@name  �ֶ�����
		bool setName(string name);

		//��ȡ�ֶ��Ƿ��Ϊ��
		bool getIsAllowNULL();

		//�����ֶ��Ƿ�Ϊ������
		//@IsNull �ֶ� trueΪ�� false ��Ϊ����Ĭ��ֵ
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
		//�����ֶ�����
		//@typeCode �ֶ�����
		bool setFieldType(Enum_FieldType typeCode);

		bool writeFieldHead(SMDStream & stream);
		bool readFieldHead(SMDStream & stream);
		//Ĭ�ϳ�ʼ������
		bool init(const string &name, Enum_FieldType Datatype,const string & Alias,
			bool IsHaveHead = false,bool ReadOnly = false, bool IsAllowNULL = true);
		//read���������
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
		//bool IsRelative; //������ֵ
	};
}

#endif
