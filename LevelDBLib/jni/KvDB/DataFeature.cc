#include "DataFeature.h"

namespace kvdbEngine
{

DataFeature::~DataFeature(void)
{
	if (pPageFile != NULL)
	{
		delete pPageFile;
		pPageFile = NULL;
	}
}

int DataFeature::addRecord( Record* record )
{
	if(record == NULL)return -1;

	if(!pPageFile->setToLastPageFile())
	{
		delete record;
		record = NULL;
		return -1;
	}
	SMDMemStream stream;
	if(!record->write(stream))
	{
		delete record;
		record = NULL;
		return -1;
	}
	//写入到文件中
	unsigned int offset = pPageFile->put(stream);
	delete record;
	return offset;
}

int DataFeature::addOneGroupRecords( vector<Record *> records )
{
	int nSize = records.size();
	if(nSize  == 0)return -1;

	if(!pPageFile->setToLastPageFile())
	{
		releaseaddRecords(records);
		return -1;
	}
	SMDMemStream stream;
	stream.writeInt(nSize);
	bool flag = false;
	
	vector<Record*>::iterator it = records.begin();
	Record * r_f = *it; 
	if (!(*it)->write(stream))
	{
		flag = true;
	}
	++it;
	for(;it!=records.end();++it)
	{
		Record * r = *it;
		Record * result = r->subRecord(r_f);
		if (!result->write(stream))
		{
			flag = true;
		}
		delete result;
		r_f = r;
	}
	if (flag == true)
	{
		releaseaddRecords(records);
		return -1;
	}

	//写入到文件中
	unsigned int offset = pPageFile->put(stream);
	releaseaddRecords(records);
	return offset;
}

vector<Record *> DataFeature::getOneGroupRecords( unsigned int offset )
{
	if(!pPageFile->setToOffset(offset))
	{
		return vector<Record *>();
	}

	SMDMemStream stream;
	if(!pPageFile->get(stream,offset))
	{
		return vector<Record *>();
	}

	if (stream.size() == 0)
	{
		return vector<Record *>();
	}

	return getOGRecordsFromStream(stream);
}
vector <Record *> DataFeature::readALL()
{
	if (tableStruct->getIsSaveByGroup())
	{
		return readALLGroupByGroup();
	}
	else
		return readALLOneByOne();
}
vector<Record * > DataFeature::readALLOneByOne()
{
	//releasereadRecords();
	//stream.setPosition(pos);
	//获取当前Table的首个pagfile
	vector <Record *> records;
	int offset =pPageFile->getNextOffset(-1);;

	//循环读取
	while(offset != -1)
	{
		int bpno = pPageFile->getPageFileNo();
		SMDMemStream stream;
		
		pPageFile->get(stream,offset);

		//读取一组record
		Record * record = getRecord(offset);
		records.push_back(record);

		unsigned int nSize = stream.size();
		int apno = pPageFile->getPageFileNo();
		if (apno == bpno)
		{
			offset += nSize + 4;
		}
		else if (apno-bpno == 1)
		{
			offset += nSize + 4 + pPageFile->getHeadSize();
		}
		else
		{
			//offset += nSize + 4 + (apno - bpno -1) * pPageFile->getPageFileSize() + pPageFile->getHeadSize();
			int currPageWr = pPageFile->getPageFileSize() - (offset % pPageFile->getPageFileSize());
			int otherPageWr = nSize + 4 - currPageWr;
			int endPageWr = otherPageWr % (pPageFile->getPageFileSize() - pPageFile->getHeadSize());
			offset = apno * pPageFile->getPageFileSize() + pPageFile->getHeadSize() + endPageWr;
		}

		offset = pPageFile->getNextOffset(offset);

	}
	return records;
}
vector<Record * > DataFeature::readALLGroupByGroup()
{
	//releasereadRecords();
	//stream.setPosition(pos);
	//获取当前Table的首个pagfile
	vector <Record *> records;
	int offset =pPageFile->getNextOffset(-1);;

	//循环读取
	int i = 0;
	while(offset != -1)
	{
		int bpno = pPageFile->getPageFileNo();
		SMDMemStream stream;
		i++;
		pPageFile->get(stream,offset);

		//读取一组record
		vector <Record *> rds = getOneGroupRecords(offset);

		//插入到整个record中
		vector <Record *>::iterator it = records.end();
		vector <Record *>::iterator itrb = rds.begin();
		vector <Record *>::iterator itre = rds.end();
		records.insert(it,itrb,itre);

		unsigned int nSize = stream.size();
		int apno = pPageFile->getPageFileNo();
		if (apno == bpno)
		{
			offset += nSize + 4;
		}
		else if (apno-bpno == 1)
		{
			offset += nSize + 4 + pPageFile->getHeadSize();
		}
		else
		{
			//offset += nSize + 4 + (apno - bpno -1) * pPageFile->getPageFileSize() + pPageFile->getHeadSize();
			int currPageWr = pPageFile->getPageFileSize() - (offset % pPageFile->getPageFileSize());
			int otherPageWr = nSize + 4 - currPageWr;
			int endPageWr = otherPageWr % (pPageFile->getPageFileSize() - pPageFile->getHeadSize());
			offset = apno * pPageFile->getPageFileSize() + pPageFile->getHeadSize() + endPageWr;
		}

		offset = pPageFile->getNextOffset(offset);

	}
	return records;
}

Record* DataFeature::getRecord( unsigned int offset )
{
	//releaseaddRecords();
	Record* record = new Record(tableStruct);
	if(record == NULL)
	{
		return NULL;
	}
	SMDMemStream stream;
	
	pPageFile->get(stream,offset);

	if(!record->read(stream))
	{
	        delete record;
		return NULL;
	}
	return record;
}

vector<Record *> DataFeature::getOGRecordsFromStream( SMDMemStream & stream )
{
	int nSize = 0;
	stream.readInt(nSize);
	vector<Record*> records;
	Record* r_f;
	for (int i =0;i<nSize; i++)
	{
		Record* r = new Record(tableStruct);
		r->read(stream);

		if (i==0)
		{
			records.push_back(r);
		}
		else
		{
			r->addRecord(r_f);
			records.push_back(r);
		}
		//cout<<"a: "<<a<<" b:"<<b<<endl;
		r_f = r;
	}
	return records;
}


void DataFeature::releaseaddRecords( vector <Record *> records )
{
	vector<Record*>::iterator it = records.begin();

	for(;it!=records.end();++it)
	{
		delete *it;
	}
	records.clear();
}

}
