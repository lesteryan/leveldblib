#include "SMDPageFile.h"
#include "SMDPageManagement.h"

namespace kvdbEngine
{
	SMDPageFile::SMDPageFile(SMDPageManagement * pageMangement,SegType enumType,string sTableName,unsigned short minUint):sTableName(sTableName)
	{
		this->pageMangement = pageMangement;
		this->enumType = enumType;
		this->minUint = minUint;
		page = NULL;
	}

	SMDPageFile::~SMDPageFile(void)
	{
		if (page)
		{
			delete page;
		}
	}

	int SMDPageFile::put( SMDMemStream & stream )
	{
		if (!stream.size())
		{
			return -1;
		}

		// ���� + ����ʵ����
		int sSize = stream.size();

		//�������Ĵ�С��Pageʣ���С�Ĺ�ϵ���趨��Ҫ����page
		//��ȡ��ǰpage
		int Pageno = pageMangement->getLastPageNo(enumType,sTableName);
		if (!pageMangement->getPageByPageNo(page,Pageno))
		{
			return -1;
		}
		
		int space = page->getSpace();
		int offset = 0;
		if (sSize + 4 <= space)
		{
			offset = page->getSpaceOffset() + page->getFileOffset();
			unsigned char buffer[4];
			SMDValue::putInt(sSize,buffer);
			page->write((const char*)buffer,4);
			page->write(stream.buffer(),stream.size());
		}
		else
		{
			int i = 0;
			bool flag = true;
			while ( i<sSize )
			{
				//�����ǰҲ��spaceС��minUint�ֽھʹ�һ���µ�ҳ,������д�굱ǰpage
				if (page->getSpace() < minUint )
				{
					pageMangement->savePageToFile(page);
					getNextPage();
					if (flag)
					{
						offset = page->getContentOffset();
						unsigned char buffer[4];
						SMDValue::putInt(sSize,buffer);
						page->write((const char*)buffer,4);
						
						flag = false;
					}
				}
				else
				{
					if (flag)
					{
						offset = page->getSpaceOffset() + page->getFileOffset();
						unsigned char buffer[4];
						SMDValue::putInt(sSize,buffer);
						page->write((const char*)buffer,4);
						
						flag = false;
					}
					int wSize = min(page->getSpace(), sSize - i);
					
					// i����Ѿ�������д�����ȵ�4���ֽ�,������Ҫ����
					page->write(stream.buffer() + i,wSize);

					i+=wSize;
					if (i != sSize)
					{
						pageMangement->savePageToFile(page);
						getNextPage();
					}
				}
			}
		}


		pageMangement->savePageToFile(page);
		
		return offset;
	}

	int SMDPageFile::put( SMDMemStream & stream,int offset )
	{
		if (!stream.size())
		{
			return -1;
		}

		if (!pageMangement->getPageByOffset(page,offset))
		{
			return -1;
		}

		int offset_tmp =offset;
		int sSize = stream.size();

		int i = 0;
		bool flag = true;
		while ( i<sSize )
		{
			if (flag)
			{
				unsigned char buffer[4];
				SMDValue::putInt(sSize,buffer);
				page->write((const char*)buffer,4,offset);
				offset += 4;
				flag = false;
			}
			int wSize = min(page->getOffsetToEndSize(offset), sSize - i);
			page->write(stream.buffer() + i,wSize,offset);

			i+=wSize;
			if (i != sSize)
			{
				pageMangement->savePageToFile(page);
				getNextPage();
				offset = page->getContentOffset();
			}
		}

		pageMangement->savePageToFile(page);
		return offset_tmp;
	}

	bool SMDPageFile::get( SMDMemStream & stream, int offset )
	{
		if (offset < 0)
		{
			setToFristPageFile();
			offset = page->getContentOffset();
		}
		else
		{
			//ͨ��offset��ȡpage
			if (!pageMangement->getPageByOffset(page,offset))
			{
				return false;
			}
		}
		offset = offset % page->getSize();
		int nSize;

		if (page->getSize() - offset < minUint)
		{
			getNextPage();
			offset = page->getContentOffset()%page->getSize();
		}
		SMDMemStream tStream;

		//��ȡ
		char p[4];
		page->read(offset,p,4);
		tStream.writeBytes(p,4);
		tStream.setPosition(0);
		tStream.readInt(nSize);
		offset += 4;
		//����ռ�
		stream.resize(nSize);
		
		//�жϵ�ǰ����ռ������page���ζ�ȡ
		int i=0;
		while (i<nSize)
		{
			int wSize = min(page->getSize()-offset, nSize - i);
			page->read(offset,(char*)stream.buffer() + i,wSize);
			i += wSize;
			if (i!=nSize)
			{
				getNextPage();
				offset = page->getContentOffset()%page->getSize();
			}
		}
		return true;
	}

	bool SMDPageFile::getNextPage()
	{
		int Pageno = page->getPageNo();
		return pageMangement->getNextPage(page,Pageno);
	}

	bool SMDPageFile::setToFristPageFile()
	{
		int Pageno = pageMangement->getFirstPageNo(enumType,sTableName);
		pageMangement->getPageByPageNo(page,Pageno);
		if (page)
		{
			return true;
		}
		return false;
	}

	bool SMDPageFile::setToLastPageFile()
	{
		int Pageno = pageMangement->getLastPageNo(enumType,sTableName);
		pageMangement->getPageByPageNo(page,Pageno);
		if (page)
		{
			return true;
		}
		return false;
	}

	bool SMDPageFile::setToOffset( unsigned int offset )
	{
		return pageMangement->getPageByOffset(page,offset);
	}

	//Page������
	bool SMDPageFile::clearPage()
	{
		setToFristPageFile();
		do 
		{
			page->freeSpace();
			this->pageMangement->savePageToFile(page);
		} while(!page->isLastPage() && getNextPage());

		return true;
	}

	int SMDPageFile::getNextOffset( int offset )
	{
		if (offset == -1)
		{
			setToFristPageFile();
			offset = page->getContentOffset();
		}
		else 
		{
			if (page->isLastPage() && offset%page->getSize() == page->getSpaceOffset())
			{
				return -1;
			}
			if (IsEndOfPageSize(offset))
			{
				getNextPage();
				offset = page->getContentOffset();
			}

		}
		return offset;
	}

}
