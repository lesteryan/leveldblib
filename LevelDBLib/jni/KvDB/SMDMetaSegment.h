#ifndef _SMD_META_SEGMENT_H_
#define _SMD_META_SEGMENT_H_

#include "SMDSegment.h"
#include "SMDPageManagement.h"

namespace kvdbEngine
{
	class SMDMetaSegment : public SMDSegment
	{
	public:
		SMDMetaSegment(SMDPageManagement* pageMgr);

		// ��ȡ���ݶ�����
		virtual SegType getType()
		{
			return SEG_TYPE_METAINFO;
		}

		// ���л����ڴ���
		virtual bool save();

		// ���ļ���ȡ���ڴ�
		// stroage�ڶ�ȡ���ݽṹʱ����ô˺���
		virtual bool read();

		// ���meta��Ϣ
		void addMetaInfo(const std::string& key,const std::string& value);

		std::string getMetaInfo(const std::string& key);
	private:
		std::map<std::string,std::string> metas;
		SMDPageManagement* pageMgr;
	};
}

#endif
