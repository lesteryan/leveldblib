#ifndef _SMD_SSTORAGE_H_
#define _SMD_SSTORAGE_H_

#include <stdio.h>
#include <string>
#include <vector>

#include "SMDMetaSegment.h"
#include "SMDDataSegment.h"
#include "SMDIndexSegment.h"
#include "SMDTableSegment.h"
#include "SMDPageManagement.h"
#include "SMDIndexSegment.h"

namespace kvdbEngine
{
	class SMDTable
	{
	public:
		friend class SMDPageManagement;

		// ����
		SMDTable();

		// ����
		~SMDTable();

		// ��Table�ļ�
		// @path �ļ�·��,�����ڻᴴ��
		// @return ���ش�table�Ƿ�ɹ�
		bool open(const std::string& path,unsigned short pageSize=4096);

		// ��Table�ļ�
		// @path �ļ�·��,�����ڻᴴ��
		// @return ���ش�table�Ƿ�ɹ�
		bool open(FILE* file,int offset,unsigned short pageSize=4096,bool exists = true);

		// �ر�Table�ļ�
		// @return �ر��Ƿ�ɹ�
		bool close();

		// ����Table�ļ�
		// @return ���ر���table�Ƿ�ɹ�
		bool save();

		// ���Meta��Ϣ
		bool addMetaInfo(const std::string& key,const std::string& val);

		// ��ȡMeta��Ϣ
		std::string getMetaInfo(const std::string& key) const;

		// ��ӱ�ṹ��Ϣ
		bool addTableStruct(TableDef *p);

		//��ȡ��ṹ��Ϣ 
		TableDef * getTableDef(const string & name);
		TableDef * getTableDef(int iIndex);

		//��ȡ��¼ 
		bool addOneGroupRecords(vector<Record *> records, const string & name);


		vector<Record *> getOneGroupRecords( unsigned int offset,const string &tableName); //��ȡһ��record	


		bool add(const string & name,Record * r,int key);
		bool add(const string & name,Record * r,const std::string& key);
		//���һ��Key�ķ�Χ
		bool addGroup(const string & name,std::vector<Record*> r,int keyS,int KeyE);


		Record * search(const string & name,int key);
		Record * search(const string & name,const std::string& key);

		std::vector<Record*> searchGroup(const string & name,int key);

		// ��ȡ���м�¼
		vector<Record* > getRecordAll(const string& tableName);

	protected:
		bool save(const void* buffer,unsigned int sz,unsigned int offset);

		bool read(void* buffer,unsigned int sz,unsigned int offset);

	private:
		//����һ����¼������
		Record * getRecord( unsigned int offset,const string &tableName ); //��ȡһ��record

		// �����ļ�ͷ��ʶ
		// @return �����Ƿ�ɹ�
		bool saveFlag(unsigned short pageSize);

		// ��֤�ļ�ͷ��ʶ
		// @return ��֤�Ƿ�ɹ�
		bool checkFlag();

	private:
		// �ƶ��ļ���ָ��λ��
		bool seekFileTo(int position);

	private:
		// �ļ�ָ��
		FILE* file;

		// �ļ�ƫ����,-1��ʾʹ��path��
		int offset;

		// ҳ�������
		SMDPageManagement* pageMgr;

		// Ԫ��Ϣ
		SMDMetaSegment* segMeta;

		// ����
		SMDDataSegment* segData;

		// ��ṹ
		SMDTableSegment* segTable;

		// ����
		SMDIndexSegment* segIndex;
	};
}

#endif
