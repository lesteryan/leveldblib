#include "SMDDataSegment.h"
#include "SMDPageFile.h"

namespace kvdbEngine
{

SMDDataSegment::SMDDataSegment(SMDTableSegment * tableSegment,SMDPageManagement *pPageManagement)
{
	this->pPageManagement = pPageManagement;
	this->tableSegment = tableSegment;
	//≥ı ºªØDataFeature
	for (int i=0;i<tableSegment->TableSum();i++)
	{
		DataFeature * p = new DataFeature(tableSegment->getTableDef(i),pPageManagement);
		Features.push_back(p);
	}
}


SMDDataSegment::~SMDDataSegment(void)
{
	ReleaseFeatures();
}


int SMDDataSegment::addRecord( Record * r,const string & tableName )
{
	for (int i=0;i<tableSegment->TableSum();i++)
	{
		string name;
		if (tableSegment->getTableName(i,name))
		{
			if (name.compare(tableName) == 0)
			{
				DataFeature * p = Features[i];
				return p->addRecord(r);
			}
		}
	}
	return -1;
}

int SMDDataSegment::addOneGroupRecords( vector<Record * > records,const string &tableName )
{
	for (int i=0;i<tableSegment->TableSum();i++)
	{
		string name;
		if (tableSegment->getTableName(i,name))
		{
			if (name.compare(tableName) == 0)
			{
				DataFeature * p = Features[i];
				return p->addOneGroupRecords(records);
			}
		}
	}
	return -1;
}

Record *SMDDataSegment::getRecord( unsigned int offset,const string &tableName )
{
	for (int i=0;i<tableSegment->TableSum();i++)
	{
		string name;
		if (tableSegment->getTableName(i,name))
		{
			if (name.compare(tableName) == 0)
			{
				DataFeature * p = Features[i];
				return  p->getRecord(offset);
			}
		}
	}
	return NULL;
}

vector<Record *> SMDDataSegment::getOneGroupRecords( unsigned int offset,const string &tableName )
{
	for (int i=0;i<tableSegment->TableSum();i++)
	{
		string name;
		if (tableSegment->getTableName(i,name))
		{
			if (name.compare(tableName) == 0)
			{
				DataFeature * p = Features[i];
				return p->getOneGroupRecords(offset); 
			}
		}
	}
    vector<Record*> a;
    return a;
}

vector<Record* > SMDDataSegment::getRecordALL( const string &tableName )
{
	for (int i=0;i<tableSegment->TableSum();i++)
	{
		string name;
		if (tableSegment->getTableName(i,name))
		{
			if (name.compare(tableName) == 0)
			{
				DataFeature * p = Features[i];
				return p->readALL(); 
			}
		}
	}

	return vector<Record* >();
}

bool SMDDataSegment::addDataFeature( TableDef * tbStruct)
{
	DataFeature * pFeature = new DataFeature(tbStruct,this->pPageManagement);
	Features.push_back(pFeature);

	if (pFeature == NULL)
	{
		return false;
	}
	return true;
}





}

