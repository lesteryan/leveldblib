#include "SMDFileStream.h"

namespace kvdbEngine
{
	SMDFileStream::SMDFileStream( FILE* file )
	{
		this->file = file;
		fileSize = 0;
	}

	SMDFileStream::~SMDFileStream()
	{

	}

	bool SMDFileStream::seekTo( unsigned int pos )
	{
		return fseek(this->file,this->pos,SEEK_SET) == 0;
	}

	unsigned int SMDFileStream::size()
	{
		if(fileSize == 0)
		{
			fseek(file,0,SEEK_END);
			fileSize = ftell(file);
		}

		return fileSize;
	}

	bool SMDFileStream::read( void* buffer,unsigned int sz )
	{
		if(this->pos + sz > size())
		{
			return false;
		}

		if(fread(buffer,1,sz,this->file) != sz)
		{
			return false;
		}

		this->pos += sz;
		return true;
	}

	bool SMDFileStream::write( const void* buffer,unsigned int sz )
	{
		if(fwrite(buffer,1,sz,this->file) != sz)
		{
			return false;
		}

		this->pos += sz;
		
		this->fileSize = std::max(this->fileSize,this->pos);

		return true;
	}
};
