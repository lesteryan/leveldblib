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

		// ��ȡ������
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

		// ��/дָ�����ȵ��ڴ�
		virtual bool read(void* buffer,unsigned int sz);
		virtual bool write(const void* buffer,unsigned int sz);

		// ������λ��
		virtual bool seekTo(unsigned int pos);

	private:
		std::string data;
	};
}

#endif
