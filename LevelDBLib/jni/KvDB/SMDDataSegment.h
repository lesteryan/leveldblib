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
		//Features 由此类释放
		SMDDataSegment(SMDTableSegment * tableSegment,SMDPageManagement *pPageManagement);
		virtual ~SMDDataSegment(void);

		// 获取内容段类型
		virtual SegType getType()
		{
			return SEG_TYPE_DATA;
		}
		//增加一条记录到table中
		int addRecord(Record * r, const string & tableName);
		int addOneGroupRecords(vector<Record * > records,const string &tableName);
		//获取一个记录
		Record * getRecord( unsigned int offset,const string &tableName );
		vector<Record *> getOneGroupRecords( unsigned int offset,const string &tableName );//获取一组record

		vector<Record* > getRecordALL(const string &tableName );

		//加入一个DataFeature
		bool addDataFeature(TableDef * tbStruct);
	protected:
		//释放内存
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
		SMDPageManagement *pPageManagement; // 构造的时候获取
		SMDTableSegment *  tableSegment;
	};
}

#endif
