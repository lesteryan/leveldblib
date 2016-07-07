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

		// 获取内容段类型
		virtual SegType getType()
		{
			return SEG_TYPE_METAINFO;
		}

		// 序列化到内存中
		virtual bool save();

		// 从文件读取到内存
		// stroage在读取数据结构时会调用此函数
		virtual bool read();

		// 添加meta信息
		void addMetaInfo(const std::string& key,const std::string& value);

		std::string getMetaInfo(const std::string& key);
	private:
		std::map<std::string,std::string> metas;
		SMDPageManagement* pageMgr;
	};
}

#endif
