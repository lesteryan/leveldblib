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

		// ��ȡ���ݶ�����
		virtual SegType getType()
		{
			return SEG_TYPE_TABLES;
		}

		// ���л����ڴ���
		virtual bool save();

		// ���ļ���ȡ���ڴ�
		// stroage�ڶ�ȡ���ݽṹʱ����ô˺���
		virtual bool read();

		//����һ����ṹ,�������ͷ�def
		bool addTableDef(TableDef* def);


		//��ȡһ����ṹ���ⲿ�����޸ģ��������ͷţ���SMDTableSegment�����ͷ�
		TableDef* getTableDef(int index);
		TableDef* getTableDef(const std::string &tableName);

		//��ȡ��index���������
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

