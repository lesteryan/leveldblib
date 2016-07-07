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

		//����һ����¼
		int addRecord(Record* record);
		int addOneGroupRecords(vector<Record *> records);


		// ��ȡ���м�¼
		vector <Record *> readALL();
		vector<Record *> getOneGroupRecords( unsigned int offset );
		
		// ����offset��ȡ��¼
		Record* getRecord(unsigned int offset);


	protected:

		//����������͵����ֵ

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

		//��ȡ���ݲ���
		vector<Record * > readALLGroupByGroup();
		vector<Record * > readALLOneByOne();
		//�ͷ�Feature
		//����pagefile
		bool setPageFile(SMDPageManagement *pPageManagement,SegType type,TableDef *ptableStruct)
		{
			this->pPageFile = pPageManagement->createPageFile(type,ptableStruct->getTableName(),0);
			if (pPageFile)
				return true;
			else	
				return false;
		}

		//����pageManagement
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
		
		//���ñ�ṹ
		bool setTableDef( TableDef * tStruct )
		{
			if (tStruct)
			{
				tableStruct = tStruct;
			}
			return false;
		}


	protected:
		TableDef * tableStruct;  //��ǰfeture��ָ��ı�ṹ
		SMDPageFile *pPageFile; // д���ļ�����
		SMDPageManagement *pPageManagement; // ��ǰ�����ļ��������
		const SegType type;
	};
}

#endif



