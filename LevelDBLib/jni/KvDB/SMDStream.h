#ifndef _SMD_STREAM_H_
#define _SMD_STREAM_H_

#include "SMDTableConfig.h"

namespace kvdbEngine
{
	class SMDStream
	{
	public:
		SMDStream(){
			pos = 0;
		}

		virtual ~SMDStream(){};

		// 移动文件指针位置
		bool setPosition(unsigned int pos)
		{
			if(pos > size())return false;
			this->pos = pos;
			return true;
		}

		// 获取文件指针位置
		unsigned int getPosition() const
		{
			return this->pos;
		}

		// 读写short
		bool writeShort(short sval);
		bool readShort(short& sval);

		// 读写无符号整数
		bool writeUShort(unsigned short ival);
		bool readUShort(unsigned short& ival);

		// 读写整数
		bool writeInt(int ival);
		bool readInt(int& ival);

		// 读写无符号整数
		bool writeUInt(unsigned int ival);
		bool readUInt(unsigned int& ival);

		// 读写long
		bool writeLong(long long lval);
		bool readLong(long long& lval);

		// 读写字符串
		bool writeString(const void* strVal,int sz);
		bool writeString(const std::string& strVal);
		bool readString(std::string& strVal);

		// 读写单精度浮点
		bool writeFloat(float fval);
		bool readFloat(float& fval);

		// 读写双精度浮点
		bool writeDouble(double fval);
		bool readDouble(double& fval);

	public:
		SMDStream& operator<<(int val)
		{
			writeInt(val);
			return *this;
		}

		SMDStream& operator<<(unsigned int val)
		{
			writeUInt(val);
			return *this;
		}

		SMDStream& operator<<(double val)
		{
			writeDouble(val);
			return *this;
		}

		SMDStream& operator<<(const std::string& val)
		{
			writeString(val);
			return *this;
		}

		SMDStream& operator>>(int& val)
		{
			readInt(val);
			return *this;
		}

		SMDStream& operator>>(unsigned int& val)
		{
			readUInt(val);
			return *this;
		}

		SMDStream& operator>>(double& val)
		{
			readDouble(val);
			return *this;
		}

		SMDStream& operator>>(std::string& val)
		{
			readString(val);
			return *this;
		}

	public:
		// 获取流长度
		virtual unsigned int size() = 0;

		bool readBytes(void* buffer,unsigned int sz)
		{
			if(!seekTo(this->pos))
			{
				return false;
			}

			return read(buffer,sz);
		}

		bool writeBytes(const void* buffer,unsigned int sz)
		{
			if(!seekTo(this->pos))
			{
				return false;
			}

			return write(buffer,sz);
		}
	protected:
		// 设置流位置
		virtual bool seekTo(unsigned int pos) = 0;

		// 读/写指定长度的内存
		virtual bool read(void* buffer,unsigned int sz) = 0;
		virtual bool write(const void* buffer,unsigned int sz) = 0;
	
		unsigned int pos;
	};
}

#endif
