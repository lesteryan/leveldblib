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

		// �ƶ��ļ�ָ��λ��
		bool setPosition(unsigned int pos)
		{
			if(pos > size())return false;
			this->pos = pos;
			return true;
		}

		// ��ȡ�ļ�ָ��λ��
		unsigned int getPosition() const
		{
			return this->pos;
		}

		// ��дshort
		bool writeShort(short sval);
		bool readShort(short& sval);

		// ��д�޷�������
		bool writeUShort(unsigned short ival);
		bool readUShort(unsigned short& ival);

		// ��д����
		bool writeInt(int ival);
		bool readInt(int& ival);

		// ��д�޷�������
		bool writeUInt(unsigned int ival);
		bool readUInt(unsigned int& ival);

		// ��дlong
		bool writeLong(long long lval);
		bool readLong(long long& lval);

		// ��д�ַ���
		bool writeString(const void* strVal,int sz);
		bool writeString(const std::string& strVal);
		bool readString(std::string& strVal);

		// ��д�����ȸ���
		bool writeFloat(float fval);
		bool readFloat(float& fval);

		// ��д˫���ȸ���
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
		// ��ȡ������
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
		// ������λ��
		virtual bool seekTo(unsigned int pos) = 0;

		// ��/дָ�����ȵ��ڴ�
		virtual bool read(void* buffer,unsigned int sz) = 0;
		virtual bool write(const void* buffer,unsigned int sz) = 0;
	
		unsigned int pos;
	};
}

#endif
