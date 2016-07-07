#ifndef _SMD_DATA_SEGMENT_H_
#define _SMD_DATA_SEGMENT_H_

#include "SMDSegment.h"
#include "SMDTableSegment.h"
#include "DataFeature.h"
#include "SMDPageManagement.h"

namespace kvdbEngine
{
	class SMDDataSegment :
		public SMDSegment
	{
	public:
		//Features �ɴ����ͷ�
		SMDDataSegment(SMDTableSegment * tableSegment,SMDPageManagement *pPageManagement);
		virtual ~SMDDataSegment(void);

		// ��ȡ���ݶ�����
		virtual SegType getType()
		{
			return SEG_TYPE_DATA;
		}
		//����һ����¼��table��
		int addRecord(Record * r, const string & tableName);
		int addOneGroupRecords(vector<Record * > records,const string &tableName);
		//��ȡһ����¼
		Record * getRecord( unsigned int offset,const string &tableName );
		vector<Record *> getOneGroupRecords( unsigned int offset,const string &tableName );//��ȡһ��record

		vector<Record* > getRecordALL(const string &tableName );

		//����һ��DataFeature
		bool addDataFeature(TableDef * tbStruct);
	protected:
		//�ͷ��ڴ�
		void ReleaseFeatures()
		{
			vector<DataFeature *>::iterator it=  Features.begin();
			for (;it!= Features.end();++it)
			{
				delete (*it);
			}
			Features.clear();
		}
	protected:
		
		vector<DataFeature *> Features;
		SMDPageManagement *pPageManagement; // �����ʱ���ȡ
		SMDTableSegment *  tableSegment;
	};
}

#endif
