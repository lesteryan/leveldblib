#ifndef _SMD_MEMSTREAM_H_
#define _SMD_MEMSTREAM_H_

#include "SMDStream.h"

namespace kvdbEngine
{
	class SMDMemStream :public SMDStream
	{
	public:
		SMDMemStream(unsigned int size = 0);
		virtual ~SMDMemStream();

		// 获取流长度
		virtual unsigned int size();

		const char* buffer() const
		{
			return data.data();
		}

		void resize(unsigned int sz)
		{
			data.resize(sz);
			setPosition(0);
		}

	protected:

		// 读/写指定长度的内存
		virtual bool read(void* buffer,unsigned int sz);
		virtual bool write(const void* buffer,unsigned int sz);

		// 设置流位置
		virtual bool seekTo(unsigned int pos);

	private:
		std::string data;
	};
}

#endif
