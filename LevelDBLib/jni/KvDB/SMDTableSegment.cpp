#include "SMDTableSegment.h"

namespace kvdbEngine
{

SMDTableSegment::SMDTableSegment(SMDPageManagement* pageMgr)
{
	this->pageMgr = pageMgr;
	Release();
	read();
}

SMDTableSegment::~SMDTableSegment(void)
{
	Release();
}

TableDef* SMDTableSegment::getTableDef(int index)
{
	int size = tableSet.size();
	if (index > size -1)
	{
		return NULL;
	}
	return tableSet[index];
}

TableDef* SMDTableSegment::getTableDef(const std::string & tableName)
{
	vector<TableDef * >::iterator iter;  
	for (iter=tableSet.begin();iter!=tableSet.end();++iter)  
	{  
		string name = (*iter)->getTableName();

		if (0 == name.compare(tableName))
			return *iter;
	}  
	return NULL;
}

bool SMDTableSegment::getTableName( int index, string & tbName )
{
	if(index < 0 || index >= tableSet.size())
		return false;
	for (int i=0;i<tableSet.size();i++)
	{
		if (i == index)
		{
			tbName = tableSet[i]->getTableName();
			return true;
		}
	}
	return false;
}

bool SMDTableSegment::addTableDef( TableDef* def )
{
	tableSet.push_back(def);
	return true;
}

bool SMDTableSegment::save()
{
	if(tableSet.size() == 0)
	{
		return true;
	}

	SMDMemStream stream;

	if(!stream.writeInt(tableSet.size()))
	{
		return false;
	}

	SMDPageFile* file = this->pageMgr->createPageFile(getType(),"table_seg",0);
	if (!file)
	{
		return false;
	}
	//if (!file->clearPage())
	//{
	//	return false;
	//}
	
	file->clearPage();

	for(vector<TableDef *>::iterator it = tableSet.begin();it != tableSet.end();++it)
	{
		(*it)->write(stream);
	}

	file->put(stream);

	delete file;
	return true;
}

bool SMDTableSegment::read()
{
	Release();
	SMDPageFile * pPageFile = pageMgr->getPageFile(this->getType(),"table_seg");
	if (!pPageFile)
	{
		return false;
	}

	SMDMemStream stream;
	int pos = stream.getPosition();
	if(!pPageFile->get(stream,-1))
	{
		return false;
	}

	stream.setPosition(pos);
	int sz = 0;

	if(!stream.readInt(sz))
	{
		return false;
	}

	for(int i=0;i<sz;i++)
	{
		TableDef * p = new TableDef("");
		p->read(stream);
		tableSet.push_back(p);
	}

	delete pPageFile;
	return true;
}

void SMDTableSegment::Release()
{
	vector<TableDef * >::iterator iter;  
	for (iter=tableSet.begin();iter!=tableSet.end();++iter)  
	{  
		TableDef * temp = *iter;
		delete temp;
	}  
	tableSet.clear();
}

}
