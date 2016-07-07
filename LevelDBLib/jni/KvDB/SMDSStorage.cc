#include "SMDSStorage.h"
#include "SMDValue.h"
#include "SMDSegmentFactory.h"
#include "SMDIndexSegment.h"

namespace kvdbEngine
{

SMDTable::SMDTable()
{
	this->file = NULL;
	this->pageMgr = NULL;
	this->segMeta = NULL;
	this->segData = NULL;
	this->segTable = NULL;
	this->segIndex = NULL;
	this->offset = -1;
}

SMDTable::~SMDTable()
{
	close();
}

bool SMDTable::open( FILE* file,int offset,unsigned short pageSize/*=4096*/,bool exists /*= true*/ )
{
	this->file = file;
	this->offset = offset;

	if(exists && !checkFlag())
	{
		return false;
	}

	if(!exists)
	{
		saveFlag(pageSize);
	}

	if(pageMgr != NULL) delete pageMgr;

	pageMgr = new SMDPageManagement(this,exists,pageSize);
	if(pageMgr == NULL)
	{
		return false;
	}

	// 初始化各个段结构
	segMeta = new SMDMetaSegment(pageMgr);
	segTable = new SMDTableSegment(pageMgr);
	segData = new SMDDataSegment(segTable,pageMgr);
	segIndex = new SMDIndexSegment(pageMgr);
	return true;
}

bool SMDTable::open( const std::string& path ,unsigned short pageSize)
{
	bool exists = true;

	file = fopen(path.c_str(),"rb");
	if(file == NULL)
	{
		file = fopen(path.c_str(),"wb+");
		if(file == NULL)return false;
		exists = false;
	}

	return open(file,-1,pageSize,exists);
}

bool SMDTable::close()
{
	if(segTable != NULL)
	{
		delete segTable;
		segTable = NULL;
	}

	if(segData != NULL)
	{
		delete segData;
		segData = NULL;
	}

	if(segMeta != NULL)
	{
		delete segMeta;
		segMeta = NULL;
	}

	if(segIndex != NULL)
	{
		delete segIndex;
		segIndex = NULL;
	}

	if(pageMgr != NULL)
	{
		delete pageMgr;
		pageMgr = NULL;
	}

	if(file != NULL && this->offset == -1)
	{
		fclose(this->file);
		this->file = NULL;
		this->offset = -1;
	}

	return true;
}

bool SMDTable::save()
{
	if(segMeta != NULL)segMeta->save();
	if (segTable != NULL)segTable->save();
	if (segIndex != NULL)segIndex->save();
	if (pageMgr != NULL)pageMgr->save();

	return true;
}

bool SMDTable::read(void* buffer,unsigned int sz,unsigned int offset )
{
	if(!seekFileTo(offset))
	{
		return false;
	}

	return fread(buffer,1,sz,file) == sz;
}

bool SMDTable::save(const void* buffer,unsigned int sz,unsigned int offset )
{
	if(!seekFileTo(offset))
	{
		return false;
	}

	return fwrite(buffer,1,sz,file) == sz;
}

bool SMDTable::saveFlag(unsigned short pageSize)
{
	SMDPage* page = new SMDPage(pageSize,0);
	if(page == NULL)
	{
		return false;
	}

	page->push();

	seekFileTo(0);
	int wr = fwrite(page->getBuffer(),1,pageSize,file);

	delete page;
	return wr == STROAGE_FLAG_LENGTH;
}

bool SMDTable::checkFlag()
{
	char buf[STROAGE_FLAG_LENGTH];
	memset(buf,0,STROAGE_FLAG_LENGTH);

	seekFileTo(0);
	int rd = fread(buf,1,STROAGE_FLAG_LENGTH,file);

	return (rd == STROAGE_FLAG_LENGTH) && (strcmp(buf,STORAGE_FLAG) == 0);
}

bool SMDTable::addMetaInfo( const std::string& key,const std::string& val )
{
	if(segMeta != NULL)
	{
		segMeta->addMetaInfo(key,val);
		return true;
	}

	return false;
}

std::string SMDTable::getMetaInfo( const std::string& key ) const
{
	if(segMeta != NULL)
	{
		return segMeta->getMetaInfo(key);
	}

	return "";
}

bool SMDTable::addTableStruct( TableDef *p )
{
	return segTable->addTableDef(p) && segData->addDataFeature(p);
}

TableDef * SMDTable::getTableDef( const string & name )
{
	return segTable->getTableDef(name);
}

TableDef * SMDTable::getTableDef( int iIndex )
{
	return segTable->getTableDef(iIndex);
}

Record *SMDTable::getRecord( unsigned int offset,const string &tableName )
{
	return segData->getRecord(offset,tableName);
}
vector<Record *> SMDTable::getOneGroupRecords( unsigned int offset,const string &tableName )
{
	return segData->getOneGroupRecords(offset,tableName);
}

vector<Record* > SMDTable::getRecordAll(const string& tableName)
{
	return segData->getRecordALL(tableName);
}

bool SMDTable::add( const string & name,Record * r, int key )
{
	if(r == NULL || name.empty())
	{
		return false;
	}

	int offset =  segData->addRecord(r,name);

	if(offset == -1)
	{
		return false;
	}

	TableDef* pTblDef = segTable->getTableDef(name);
	std::string sIndexName = name + "Index";
	return segIndex->insertKeyOffset<int,int>(sIndexName,key,offset,pTblDef->getIndexType());
}
bool SMDTable::add( const string & name,Record * r, const std::string& key )
{
	if(r == NULL || name.empty())
	{
		return false;
	}

	int offset =  segData->addRecord(r,name);

	if(offset == -1)
	{
		return false;
	}

	TableDef* pTblDef = segTable->getTableDef(name);
	std::string sIndexName = name + "Index";
	return segIndex->insertKeyOffset<std::string,std::string>(sIndexName,key,offset,pTblDef->getIndexType());
}
bool SMDTable::addGroup(const string & name,std::vector<Record*> r,int keyS,int KeyE)
{
	if(r.empty() || name.empty())
	{
		return false;
	}

	int offset =  segData->addOneGroupRecords(r,name);

	if(offset == -1)
	{
		return false;
	}

	TableDef* pTblDef = segTable->getTableDef(name);
	std::string sIndexName = name + "Index";
	PairKey_t<int> pairKey(keyS,KeyE);
	return segIndex->insertKeyOffset<int,PairKey_t<int> >(sIndexName,pairKey,offset,pTblDef->getIndexType());
}

std::vector<Record*> SMDTable::searchGroup(const string & name,int key)
{
	TableDef* pTblDef = segTable->getTableDef(name);
	std::string sIndexName = name + "Index";
	int offset = segIndex->searchFromFile<int>(sIndexName,key,pTblDef->getIndexType());
	if(offset == -1)
	{
		return std::vector<Record*>();
	}

	return segData->getOneGroupRecords(offset,name);
}

Record * SMDTable::search( const string& name,int key )
{
	TableDef* pTblDef = segTable->getTableDef(name);
	std::string sIndexName = name + "Index";
	int offset = segIndex->searchFromFile<int>(sIndexName,key,pTblDef->getIndexType());
	if(offset == -1)
	{
		return NULL;
	}
	return segData->getRecord(offset,name);
}
Record * SMDTable::search( const string& name,const std::string& key )
{
	TableDef* pTblDef = segTable->getTableDef(name);
	std::string sIndexName = name + "Index";
	int offset = segIndex->searchFromFile<std::string>(sIndexName,key,pTblDef->getIndexType());
	if(offset == -1)
	{
		return NULL;
	}
	return segData->getRecord(offset,name);
}

bool SMDTable::addOneGroupRecords( vector<Record *> records, const string & name )
{
	if (records.size() == 0)
	{
		return false;
	}
	int offset = segData->addOneGroupRecords(records,name);

	if (-1 != offset)
	{
		return true;
	}
	else return false;
}

bool SMDTable::seekFileTo( int position )
{
	if(this->offset > 0)
	{
		return fseek(file,this->offset + position,SEEK_SET) == 0;
	}
	else
	{
		return fseek(file,position,SEEK_SET) == 0;
	}
}

}//namespace kvdbEngine

