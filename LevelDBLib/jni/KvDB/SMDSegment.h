#ifndef _SMD_SEGMENT_H_
#define _SMD_SEGMENT_H_

#include "SMDTableConfig.h"

namespace kvdbEngine
{
	typedef enum
	{
		SEG_TYPE_NONE = 0,
		// Ԫ��Ϣ
		SEG_TYPE_METAINFO = 1,
		// ����
		SEG_TYPE_INDEX = 2,
		// ��ṹ
		SEG_TYPE_TABLES = 3,
		// ���ݶ�
		SEG_TYPE_DATA = 4,
		// �ļ��洢�ṹ
		SEG_TYPE_STORAGE = 5,
		// ҳ����洢��
		SEG_TYPE_PAGEMGR = 6
	}SegType;

	class SMDSegment
	{
	public:
		virtual ~SMDSegment(){}

		// ��ȡ���ݶ�����
		virtual SegType getType() = 0;
	};
}

#endif
