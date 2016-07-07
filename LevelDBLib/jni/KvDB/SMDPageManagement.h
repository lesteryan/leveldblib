#ifndef _SMD_PAGEMANAGEMENT_H
#define _SMD_PAGEMANAGEMENT_H

#include <string>
#include <map>
using namespace std;

#include "SMDPage.h"
#include "SMDSegment.h"
#include "SMDPageFile.h"
#include "SMDFileStream.h"
#include "SMDPageManagement.h"
#include "dev_buffer.h"

namespace kvdbEngine
{

#define SMD_PAGE_SIZE 4096

class SMDTable;

class SMDPageManagement
{
public:
	SMDPageManagement(SMDTable* table, bool exists, unsigned short nPageSize=SMD_PAGE_SIZE);
	virtual ~SMDPageManagement(void);

public:
	//returned pointer should be released outside when used.
	SMDPageFile* getPageFile(SegType enumType,string sTableName);

	//create one, and all page number increasing one,
	//assign the increasing page number to the table's FirstPageNo and serialize 
	SMDPageFile* createPageFile(SegType enumType,string sTableName,unsigned short minUnit);

	//return pageNo. return -1 mains an error
	int getFirstPageNo(SegType enumType,string sTableName);

	//return pageNo. if there is not exist the table name, then return -1
	int getLastPageNo(SegType enumType,string sTableName);

	//if nPageNo is less than All page number, read page info from file;
	//if return true, pls remember to release the pPage,after using.
	bool getNextPage(SMDPage*& pPage, unsigned int nPageNo);

	//get page info from page number
	//if return true, pls remember to release the pPage,after using.
	bool getPageByPageNo(SMDPage*& pPage, unsigned int nPageNo); 

	//get page info from file offset
	//if return true, pls remember to release the pPage,after using.
	bool getPageByOffset(SMDPage*& pPage, unsigned int nOffset); 

	//save page to file
	bool savePageToFile(SMDPage*& pPage); 

	//get all page count
	unsigned int getAllPageCnt(){return m_nPageCnt;};

	//get m_nPageSize
	unsigned short getPageSize(){return m_nPageSize;};

	// 保存系统表结构
	bool save();
protected:
	//save page management to file
	bool saveInnerInfoToFile();
	//read page management from file
	bool getInnerInfoFromFile(bool exists);

	void incLastPageNo(unsigned int lastPageNo,unsigned int newPageNo);

	// 预留扩展字段
	bool readExternAttributes(SMDStream& stream);
	bool writeExternAttributes(SMDStream& stream);
private:
	//inner container to maintain page number infomations;
	typedef struct _PageKey_t
	{
		SegType segType;
		string sTableName;

		_PageKey_t()
		{
			segType = SEG_TYPE_NONE;
		};

		_PageKey_t(SegType sType,string sTableName)
		{
			this->segType = sType;
			this->sTableName = sTableName;
		};

		bool operator<(const _PageKey_t& pageKey) const
		{
			return this->segType < pageKey.segType || 
				(this->segType == pageKey.segType && this->sTableName < pageKey.sTableName);
		}
	}PageKey_t;

	typedef struct _PageNo_t
	{
		unsigned short nOffset;
		unsigned short nMinUnit;
		unsigned int nRootPageNo;
		unsigned int nLastPageNo;

		_PageNo_t()
		{
			nRootPageNo = 0;
			nLastPageNo = 0;
			nOffset = 0;
			nMinUnit = 0;
		};
		_PageNo_t(unsigned int nRoot,unsigned short offset,unsigned int nLast,unsigned short minUnit)
		{
			this->nRootPageNo = nRoot;
			this->nLastPageNo = nLast;
			this->nOffset = offset;
			this->nMinUnit = minUnit;
		};
	}PageNo_t;
	std::map<SMDPageManagement::PageKey_t,SMDPageManagement::PageNo_t> m_PageInfo;

	//page size define
	unsigned short m_nPageSize;
	//file pointer
	SMDTable* table;

	CDevCache m_oDevCache;

	//all pages of the file
	unsigned int m_nPageCnt;

	std::map<std::string,std::string> externAttributes;
};


}//namespace kvdbEngine


#endif //_SMD_PAGEMANAGEMENT_H
