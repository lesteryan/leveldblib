#include "SMDMemStream.h"

namespace kvdbEngine
{
	SMDMemStream::SMDMemStream(unsigned int size ) : data(size,'\0')
	{

	}

	SMDMemStream::~SMDMemStream()
	{

	}

	bool SMDMemStream::seekTo( unsigned int pos )
	{
		return true;
	}

	unsigned int SMDMemStream::size()
	{
		return data.size();
	}

	bool SMDMemStream::read(void* buffer,unsigned int sz )
	{
		if(this->pos + sz > size())
		{
			return false;
		}

		memcpy(buffer,data.data()+pos,sz);
		this->pos += sz;
		return true;
	}

	bool SMDMemStream::write( const void* buffer,unsigned int sz )
	{
		if(this->pos + sz > size())
		{
			data.resize(this->pos + sz);
		}

		memcpy((char*)data.data()+pos,buffer,sz);
		this->pos += sz;
		return true;
	}
};
