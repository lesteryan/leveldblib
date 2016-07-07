#ifndef _SMD_PAGE_FILE_H_
#define _SMD_PAGE_FILE_H_

#include "SMDMemStream.h"
#include "SMDPage.h"
#include "SMDSegment.h"


#include <string>
using namespace std;

namespace kvdbEngine
{
	class SMDPageManagement;

	class SMDPageFile
	{
	public:
		SMDPageFile(SMDPageManagement * pageMangement,SegType enumType,string sTableName,unsigned short minUint);
		~SMDPageFile(void);

		//写入流
		int put( SMDMemStream & stream);

		//写入流
		int put(SMDMemStream & stream,int offset);
		//读取流
		bool get(SMDMemStream & stream, int  offset);

		//获取下一个page
		bool getNextPage();
		int getNextOffset(int offset);

		//设置当前page到第一个位置
		bool setToFristPageFile();

		//设置page到当前类型的最后
		bool setToLastPageFile();
		int  getPageFileNo()
		{
			return page->getPageNo();
		}
		//设置到特定的offset
		bool setToOffset(unsigned int offset);

		bool clearPage();
		//设置当前page到第一个位置
		//bool setFristPageFile(SegType enumType,string sTableName);
		//获取文件尾
		bool IsEndOfPageSize(unsigned int offset)
		{
			if ( (page->getPageNo()+ 1) * page->getSize() - offset < minUint)
			{
				return true;
			}
			return false;
		}
		int getHeadSize()
		{
			return page->getContentOffset() - page->getFileOffset();
		}
		bool IsLastPage()
		{
			return page->isLastPage();	
		}

		//获取pagefile的offset,写不写的第一个位置
		unsigned int getPageFileBeginOffset() 
		{
			if(page == NULL)
			{
				setToFristPageFile();
			}
			return  page->getContentOffset();
		}
		int getPageNo()
		{
			return page->getPageNo();
		}
		unsigned int getPageFileSpaceOffset()
		{
			if (page->getSpaceOffset()<minUint)
			{
				getNextPage();
			}
			return page->getSpaceOffset() + page->getFileOffset();
		}
		unsigned int  getPageFileSize()
		{
			return page->getSize();
		}
	protected: 
		
		int min(int a,int b)
		{
			return (a < b)?a:b;
		}
	protected:
		SegType enumType;
		string sTableName;
		SMDPageManagement * pageMangement;
		SMDPage *page;	
		unsigned short minUint; 
	};
}

#endif


