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

		// ��ȡ������
		virtual unsigned int size();
	
	protected:
		// ��/дָ�����ȵ��ڴ�
		virtual bool read(void* buffer,unsigned int sz);
		virtual bool write(const void* buffer,unsigned int sz);

		// ������λ��
		virtual bool seekTo(unsigned int pos);

	private:
		FILE* file;
		unsigned int fileSize;
	};
}

#endif
