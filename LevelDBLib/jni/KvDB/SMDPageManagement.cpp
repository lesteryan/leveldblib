#include "SMDPageManagement.h"
#include "SMDMemStream.h"
#include "SMDFileStream.h"
#include "SMDSStorage.h"

namespace kvdbEngine
{

#define PAGE_MANAGEMENT_NAME ("smd_pagemgr")

SMDPageManagement::SMDPageManagement(SMDTable* table, bool exists, unsigned short nPageSize)
                                   :m_oDevCache(nPageSize)
{
	m_nPageSize = nPageSize;

	m_nPageCnt = 0;

	this->table = table;
        m_oDevCache.init(table->file);

	//get all page management table from file
	getInnerInfoFromFile(exists);
	
}
SMDPageManagement::~SMDPageManagement(void)
{
	m_PageInfo.clear();
}

//Page management storage
//the first line record some meta info of itself.
// | column2(1Byte) | column3(var)             | column4(4Bytes) | column5(4Bytes)
// | enum type      | table name(nust<249chars)| root page No.   | last page No.
// | SEG_TYPE_NONE  | SMD_ALLPAGECNT_NAME      |  all line count | all page count     <---------first line is meta
// | SEG_TYPE_DATA  | "PoiTable"               |      5          | 24521                 
bool SMDPageManagement::getInnerInfoFromFile(bool exists)
{
	// ¿ªÍ·×Ö¶ÎÃèÊö
	// byte Òýµ¼ 3byte(Ô¤Áô) | short pagesize  | int pagecount | int info size
	// 
	m_PageInfo.clear();

	PageKey_t key;
	PageNo_t value;

	key.segType = SEG_TYPE_PAGEMGR;
	key.sTableName = PAGE_MANAGEMENT_NAME;

	value.nLastPageNo  = 0;
	value.nOffset = 0;
	value.nRootPageNo = 0;

	m_PageInfo.insert(std::pair<PageKey_t,PageNo_t>(key,value));

	if(!exists)
	{
		return true;
	}

	SMDPageFile* file = getPageFile(SEG_TYPE_PAGEMGR,PAGE_MANAGEMENT_NAME);
	if(file == NULL)
	{
		return false;
	}

	SMDMemStream memStream;
	if(!file->get(memStream,-1))
	{
		delete file;
		return false;
	}

	delete file;

	char beg = 0;
	if(!memStream.readBytes(&beg,1) || beg == 0)
	{
		return true;
	}
	
	// Ìø¹ýÔ¤Áô×Ö½Ú
	memStream.setPosition(4);

	if(!memStream.readUShort(m_nPageSize))
	{
		return false;
	}

	if(!memStream.readUInt(m_nPageCnt))
	{
		return false;
	}

	int size = 0;
	if(!memStream.readInt(size))
	{
		return false;
	}

	readExternAttributes(memStream);

	char type = 0;
	for(int i=0;i<size;i++)
	{
        PageKey_t key;
        PageNo_t value;
		if(!memStream.readBytes(&type,1))
		{
			return false;
		}

		key.segType = (SegType)type;

		if(!memStream.readString(key.sTableName))
		{
			return false;
		}

		if(!memStream.readUInt(value.nRootPageNo))
		{
			return false;
		}

		if(!memStream.readUShort(value.nOffset))
		{
			return false;
		}

		if(!memStream.readUShort(value.nMinUnit))
		{
			return false;
		}

		if(!memStream.readUInt(value.nLastPageNo))
		{
			return false;
		}

		if(m_PageInfo.find(key) != m_PageInfo.end())
		{
			m_PageInfo[key] = value;
		}
		else
		{
			m_PageInfo.insert(std::pair<PageKey_t,PageNo_t>(key,value));
		}
	}

	return true;
}

bool SMDPageManagement::saveInnerInfoToFile()
{
	SMDPageFile* file = getPageFile(SEG_TYPE_PAGEMGR,PAGE_MANAGEMENT_NAME);
	if(file == NULL)
	{
		return false;
	}

	SMDMemStream memStream;
	memStream.resize(4);

	char beg = 1;
	if(!memStream.writeBytes(&beg,1))
	{
        delete file;
		return false;
	}

	// Ìø¹ýÔ¤Áô×Ö½Ú
	memStream.setPosition(4);


	if(!memStream.writeUShort(m_nPageSize))
	{
        delete file;
		return false;
	}

	if(!memStream.writeUInt(m_nPageCnt))
	{
        delete file;
		return false;
	}

	if(!memStream.writeInt(m_PageInfo.size()))
	{
        delete file;
		return false;
	}

	writeExternAttributes(memStream);

	map< SMDPageManagement::PageKey_t, SMDPageManagement::PageNo_t>::iterator iter =
		m_PageInfo.begin();

	for(;iter != m_PageInfo.end();++iter)
	{
		if(!memStream.writeBytes(&(iter->first.segType),1))
		{
            delete file;
			return false;
		}


		if(!memStream.writeString(iter->first.sTableName))
		{
            delete file;
			return false;
		}

		if(!memStream.writeUInt(iter->second.nRootPageNo))
		{
            delete file;
			return false;
		}

		if(!memStream.writeUShort(iter->second.nOffset))
		{
            delete file;
			return false;
		}

		if(!memStream.writeUShort(iter->second.nMinUnit))
		{
            delete file;
			return false;
		}

		if(!memStream.writeUInt(iter->second.nLastPageNo))
		{
            delete file;
			return false;
		}
	}

	file->clearPage();
	file->put(memStream);

	delete file;
	return true;
}

SMDPageFile* SMDPageManagement::getPageFile(SegType enumType,string sTableName)
{
	map<SMDPageManagement::PageKey_t,SMDPageManagement::PageNo_t>::iterator iterM;
	iterM = m_PageInfo.find(PageKey_t(enumType,sTableName));
	if(iterM != m_PageInfo.end())
	{
		return new SMDPageFile(this,enumType,sTableName,iterM->second.nMinUnit);
	}
	else
	{
		return NULL;
	}
}

SMDPageFile* SMDPageManagement::createPageFile(SegType enumType,string sTableName,unsigned short minUnit)
{
	if (minUnit < 4)
	{
		minUnit = 4;
	}
	//search table's name from m_PageInfo
	map<SMDPageManagement::PageKey_t,SMDPageManagement::PageNo_t>::iterator iterM;
	iterM = m_PageInfo.find(PageKey_t(enumType,sTableName));
	if (iterM!=m_PageInfo.end()) //the page is already exist
	{
		return getPageFile(enumType,sTableName);
	}
	else //create new one 
	{
		//all count increase
		m_nPageCnt++;
		//insert into page table
		m_PageInfo.insert(make_pair<SMDPageManagement::PageKey_t,SMDPageManagement::PageNo_t> 
			(PageKey_t(enumType,sTableName),PageNo_t(m_nPageCnt,0,m_nPageCnt,minUnit)));

		//the record <PageKey_t(SEG_TYPE_NONE,SMD_ALLPAGECNT_NAME),PageNo_t(LineCnt,LastNo)> 
		//mains page management line count and the hole file's last page number.
		if (!saveInnerInfoToFile())
			return NULL;

		SMDPage* page = new SMDPage(m_nPageSize,m_nPageCnt);
		if(!savePageToFile(page))
		{
			delete page;
			page = NULL;
			return NULL;
		}
		delete page;
		return getPageFile(enumType,sTableName);
	}
}
int SMDPageManagement::getFirstPageNo(SegType enumType,string sTableName)
{
	//search table's name from m_PageInfo
	map<SMDPageManagement::PageKey_t,SMDPageManagement::PageNo_t>::iterator iterM;
	iterM = m_PageInfo.find(PageKey_t(enumType,sTableName));
	if (iterM!=m_PageInfo.end()) //get the table
	{
		return iterM->second.nRootPageNo;
	}
	else //can't get the first page than create
	{
		//@todo: Create new page, insert the page info into m_PageInfo
	
		//@todo: serialize the changed m_PageInfo

		return -1;
	}
}
int SMDPageManagement::getLastPageNo(SegType enumType,string sTableName)
{
	//search table's name from m_PageInfo
	map<SMDPageManagement::PageKey_t,SMDPageManagement::PageNo_t>::iterator iterM;
	iterM = m_PageInfo.find(PageKey_t(enumType,sTableName));
	if (iterM!=m_PageInfo.end()) //get the table
	{
		return iterM->second.nLastPageNo;
	}
	else //can't get the last page number,return -1
	{
		return -1;
	}
}

bool SMDPageManagement::getNextPage(SMDPage*& pSMDPage, unsigned int nPageNo)
{
	if(pSMDPage == NULL)
	{
		pSMDPage = new SMDPage(m_nPageSize,0xFFFFFFFF);
	}

	if(pSMDPage->getPageNo() != nPageNo)
	{
		if(!getPageByPageNo(pSMDPage,nPageNo))
		{
			return false;
		}
	}

	unsigned int nextPageno = pSMDPage->getNextPageNo();
	if(nextPageno != 0xFFFFFFFF)
	{
		return getPageByPageNo(pSMDPage,nextPageno);
	}
	else
	{
		m_nPageCnt++;

		pSMDPage->setNextPageNo(m_nPageCnt);

		if(!savePageToFile(pSMDPage))
		{
			return false;
		}

		incLastPageNo(pSMDPage->getPageNo(),m_nPageCnt);

		pSMDPage->reset(m_nPageSize,m_nPageCnt);

		if(!savePageToFile(pSMDPage))
		{
			return false;
		}

		return true;
	}
}

bool SMDPageManagement::getPageByPageNo(SMDPage*& pSMDPage, unsigned int nPageNo)
{
	if(pSMDPage == NULL)
	{
		pSMDPage = new SMDPage(m_nPageSize,0xFFFFFFFF);
	}

	if(pSMDPage->getPageNo() != nPageNo)
	{
		pSMDPage->reset(m_nPageSize,nPageNo);

        const char* pcPageData = m_oDevCache.getPageByNo(nPageNo);
        if (NULL == pcPageData) {
            return false;
        }
        memcpy((void*)pSMDPage->getBuffer(), pcPageData, m_nPageSize);
    #if 0
		unsigned int offset = pSMDPage->getFileOffset();

		if(!table->read((char*)pSMDPage->getBuffer(),m_nPageSize,offset))
		{
			return false;
		}
    #endif

		pSMDPage->pop();
	}

	return true;
}

bool SMDPageManagement::getPageByOffset(SMDPage*& pSMDPage, unsigned int nOffset)
{
	return getPageByPageNo(pSMDPage, nOffset / m_nPageSize);
}

bool SMDPageManagement::savePageToFile(SMDPage*& page)
{
	if(!page->isModify())return true;

	page->push();

	unsigned int offset = page->getFileOffset();

	if(!table->save((char*)page->getBuffer(),m_nPageSize,offset))
	{
		return false;
	}

	page->setModify(false);
	return true;
}

void SMDPageManagement::incLastPageNo(unsigned int lastPageNo,unsigned int newPageNo)
{
	map<SMDPageManagement::PageKey_t,SMDPageManagement::PageNo_t>::iterator iterM = m_PageInfo.begin();
	for(;iterM != m_PageInfo.end();iterM++)
	{
		if(iterM->second.nLastPageNo == lastPageNo)
		{
			iterM->second.nLastPageNo = newPageNo;
			return;
		}
	}

	//assert(0);
}

bool SMDPageManagement::readExternAttributes(SMDStream& stream)
{
	int size = 0;
	stream.readInt(size);

	for(int i=0;i<size;i++)
	{
		std::string key;
		std::string val;
		stream.readString(key);
		stream.readString(val);

		externAttributes.insert(std::pair<std::string,std::string>(key,val));
	}

	return true;
}

bool SMDPageManagement::writeExternAttributes(SMDStream& stream)
{
	stream.writeInt(externAttributes.size());
	if(!externAttributes.empty())
	{
		for(std::map<std::string,std::string>::iterator it = externAttributes.begin();it != externAttributes.end();++it)
		{
			stream.writeString(it->first);
			stream.writeString(it->second);
		}
	}

	return true;
}

bool SMDPageManagement::save()
{
	return saveInnerInfoToFile();
}
}
