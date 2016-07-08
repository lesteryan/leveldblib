#include "SMDMetaSegment.h"
#include "SMDMemStream.h"
#include "SMDFileStream.h"

namespace kvdbEngine
{

bool SMDMetaSegment::save()
{
	if(metas.empty())
	{
		return true;
	}

	SMDMemStream stream;

	if(!stream.writeInt(metas.size()))
	{
		return false;
	}

	for(std::map<std::string,std::string>::iterator it = metas.begin();it != metas.end();++it)
	{
		if(!stream.writeString(it->first) || !
			stream.writeString(it->second))
		{
			return false;
		}
	}

	SMDPageFile* file = this->pageMgr->createPageFile(getType(),"smd_meta",0);
	if(file == NULL)
	{
		return false;
	}

	file->clearPage();
	file->put(stream);
	delete file;
	return true;
}

void SMDMetaSegment::addMetaInfo( const std::string& key,const std::string& value )
{
	std::map<std::string,std::string>::iterator it = metas.find(key);

	if(it != metas.end())
	{
		metas[key] = value; 
	}
	else
	{
		metas.insert(std::pair<std::string,std::string>(key,value));
	}
}

std::string SMDMetaSegment::getMetaInfo( const std::string& key )
{
	std::map<std::string,std::string>::iterator it = metas.find(key);

	if(it != metas.end())
	{
		return metas[key];
	}
	else
	{
		return "";
	}
}

bool SMDMetaSegment::read()
{
	SMDPageFile* file = this->pageMgr->getPageFile(getType(),"smd_meta");
	if(file == NULL)
	{
		return false;
	}

	SMDMemStream stream;
	if(!file->get(stream,-1))
	{
		return false;
	}

	stream.setPosition(0);
	int sz = 0;
	if(!stream.readInt(sz) || sz > MAX_META_INFO_COUNT)
	{
		return false;
	}

	for(int i=0;i<sz;i++)
	{
        // linux平台放在外边变量会一直被沿用
        std::string strVal;
        std::string strKey;

		if(!stream.readString(strKey) || !stream.readString(strVal))
		{
			return false;
		}

		addMetaInfo(strKey,strVal);
	}

	delete file;
	return true;
}

SMDMetaSegment::SMDMetaSegment( SMDPageManagement* pageMgr )
{
	this->pageMgr = pageMgr;
	read();
}

}
