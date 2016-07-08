#include "SMDStream.h"
#include "SMDValue.h"

namespace kvdbEngine
{
	bool SMDStream::writeUInt( unsigned int ival )
	{
		unsigned char buffer[4];
		SMDValue::putInt(ival,buffer);
		return writeBytes(buffer,4);
	}

	bool SMDStream::readUInt( unsigned int& ival )
	{
		unsigned char buffer[4];
		bool rd = readBytes(buffer,4);
		if (!rd) return false;

		ival = SMDValue::getInt(buffer);
		return true;
	}

	bool SMDStream::writeInt( int ival )
	{
		unsigned char buffer[4];
		SMDValue::putInt(ival,buffer);
		return writeBytes(buffer,4);
	}

	bool SMDStream::readInt( int& ival )
	{
		unsigned char buffer[4];
		bool rd = readBytes(buffer,4);
		if (!rd) return false;

		ival = SMDValue::getInt(buffer);
		return true;
	}

	bool SMDStream::writeString(const void* strVal,int sz)
	{
		if(!writeInt(sz))
		{
			return false;
		}

		if(!writeBytes(strVal,sz))
		{
			return false;
		}
		return true;
	}

	bool SMDStream::writeString( const std::string& strVal )
	{
		return writeString(strVal.c_str(),strVal.size());
	}

	bool SMDStream::readString( std::string& strVal )
	{
		int sz = 0;
		if(!readInt(sz))
		{
			return false;
		}

		strVal.resize(sz);

		if(!readBytes((char*)strVal.data(),sz))
		{
			return false;
		}

		return true;
	}

	bool SMDStream::writeFloat( float fval )
	{
		unsigned char buffer[4];
		SMDValue::putFloat(fval,buffer);
		return writeBytes(buffer,4);
	}

	bool SMDStream::readFloat( float& fval )
	{
		unsigned char buffer[4];
		bool rd = readBytes(buffer,4);
		if (!rd) return false;

		fval = SMDValue::getFloat(buffer);
		return true;
	}

	bool SMDStream::writeDouble( double dval )
	{
		unsigned char buffer[8];
		SMDValue::putDouble(dval,buffer);
		return writeBytes(buffer,8);
	}

	bool SMDStream::readDouble( double& dval )
	{
		unsigned char buffer[8];
		bool rd = readBytes(buffer,8);
		if (!rd) return false;

		dval = SMDValue::getDouble(buffer);
		return true;
	}

	bool SMDStream::writeUShort( unsigned short sval )
	{
		unsigned char buffer[2];
		SMDValue::putShort(sval,buffer);
		return writeBytes(buffer,2);
	}

	bool SMDStream::readUShort( unsigned short& sval )
	{
		unsigned char buffer[2];
		bool rd = readBytes(buffer,2);
		if (!rd) return false;

		sval = SMDValue::getShort(buffer);
		return true;
	}

	bool SMDStream::writeShort( short sval )
	{
		unsigned char buffer[2];
		SMDValue::putShort(sval,buffer);
		return writeBytes(buffer,2);
	}

	bool SMDStream::readShort( short& sval )
	{
		unsigned char buffer[2];
		bool rd = readBytes(buffer,2);
		if (!rd) return false;

		sval = SMDValue::getShort(buffer);
		return true;
	}

	bool SMDStream::writeLong( long long lval )
	{
		unsigned char buffer[8];
		SMDValue::putLong(lval,buffer);
		return writeBytes(buffer,8);
	}

	bool SMDStream::readLong( long long& lval )
	{
		unsigned char buffer[8];
		bool rd = readBytes(buffer,8);
		if (!rd) return false;

		lval = SMDValue::getLong(buffer);
		return true;
	}

}
