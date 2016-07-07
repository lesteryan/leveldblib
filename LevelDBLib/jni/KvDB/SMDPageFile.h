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

		//д����
		int put( SMDMemStream & stream);

		//д����
		int put(SMDMemStream & stream,int offset);
		//��ȡ��
		bool get(SMDMemStream & stream, int  offset);

		//��ȡ��һ��page
		bool getNextPage();
		int getNextOffset(int offset);

		//���õ�ǰpage����һ��λ��
		bool setToFristPageFile();

		//����page����ǰ���͵����
		bool setToLastPageFile();
		int  getPageFileNo()
		{
			return page->getPageNo();
		}
		//���õ��ض���offset
		bool setToOffset(unsigned int offset);

		bool clearPage();
		//���õ�ǰpage����һ��λ��
		//bool setFristPageFile(SegType enumType,string sTableName);
		//��ȡ�ļ�β
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

		//��ȡpagefile��offset,д��д�ĵ�һ��λ��
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


