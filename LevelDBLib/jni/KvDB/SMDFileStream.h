#ifndef _SMD_FILESTREAM_H_
#define _SMD_FILESTREAM_H_

#include "SMDStream.h"

namespace kvdbEngine
{
	class SMDFileStream : public SMDStream
	{
	public:
		SMDFileStream(FILE* file);

		virtual ~SMDFileStream();

		// 获取流长度
		virtual unsigned int size();
	
	protected:
		// 读/写指定长度的内存
		virtual bool read(void* buffer,unsigned int sz);
		virtual bool write(const void* buffer,unsigned int sz);

		// 设置流位置
		virtual bool seekTo(unsigned int pos);

	private:
		FILE* file;
		unsigned int fileSize;
	};
}

#endif
