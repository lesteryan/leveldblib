#ifndef _SMD_SEGMENT_H_
#define _SMD_SEGMENT_H_

#include "SMDTableConfig.h"

namespace kvdbEngine
{
	typedef enum
	{
		SEG_TYPE_NONE = 0,
		// 元信息
		SEG_TYPE_METAINFO = 1,
		// 索引
		SEG_TYPE_INDEX = 2,
		// 表结构
		SEG_TYPE_TABLES = 3,
		// 数据段
		SEG_TYPE_DATA = 4,
		// 文件存储结构
		SEG_TYPE_STORAGE = 5,
		// 页管理存储段
		SEG_TYPE_PAGEMGR = 6
	}SegType;

	class SMDSegment
	{
	public:
		virtual ~SMDSegment(){}

		// 获取内容段类型
		virtual SegType getType() = 0;
	};
}

#endif
