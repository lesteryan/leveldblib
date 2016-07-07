#ifndef _SMD_PAGE_H_
#define _SMD_PAGE_H_

#include "SMDTableConfig.h"
#include "SMDValue.h"

namespace kvdbEngine
{

typedef struct
{
	// �ļ�ҳʣ��ռ�
	unsigned short space;

	// ���������ֽ�
	unsigned short reserve1;

	// ���������ֽ�
	unsigned int reserve2;

	// ��һ���ļ�ҳ���
	unsigned int next;
}SMDPageHead;

class SMDPage
{
public:
	SMDPage(unsigned short size,unsigned int pageno)
		: data(size,'\0')
	{
		this->modify = true;
		this->size = size;
		this->pageno = pageno;

		head.space = size - sizeof(head);
		head.reserve1 = 0;
		head.reserve2 = 0;
		head.next = 0xFFFFFFFF;

		checkFirstPage();
	}
	
	void reset(unsigned short size,unsigned int pageno)
	{
		this->modify = true;
		this->size = size;
		this->pageno = pageno;

		head.space = size - sizeof(head);
		head.reserve1 = 0;
		head.reserve2 = 0;
		head.next = 0xFFFFFFFF;

		memset((char*)data.data(),0,data.size());

		checkFirstPage();
	}

	~SMDPage()
	{

	}

	// �ͷ�ҳ�ռ�
	void freeSpace()
	{
		this->modify = true;
		head.space = size - sizeof(head);

		memset((char*)data.data(),0,data.size());

		checkFirstPage();
	}

	// �Ƿ����޸�
	bool isModify() const
	{
		return modify;
	}

	// ����Modify
	void setModify(bool modify)
	{
		this->modify = modify;
	}

	// ���ڴ��лָ��ļ��ṹ
	// ��ȡҳͷ��Ϣ
	bool pop()
	{
		int headOffset = 0;
		if(pageno == 0)
		{
			headOffset = STROAGE_FLAG_LENGTH;
		}

		head.next = SMDValue::getInt((unsigned char*)getBuffer() + headOffset);
		head.space = SMDValue::getShort((unsigned char*)getBuffer() + sizeof(int) + headOffset);
		head.reserve1 = SMDValue::getShort((unsigned char*)getBuffer() + sizeof(int) + sizeof(short) + headOffset);
		head.reserve2 = SMDValue::getShort((unsigned char*)getBuffer() + sizeof(int) + sizeof(short) + sizeof(int) + headOffset);
		return true;
	}

	// �洢�ļ��ṹ���ڴ���
	// ����ҳͷ��Ϣ
	bool push()
	{
		int headOffset = 0;
		if(pageno == 0)
		{
			headOffset = STROAGE_FLAG_LENGTH;
			memcpy((unsigned char*)getBuffer(),STORAGE_FLAG,strlen(STORAGE_FLAG));
		}

		SMDValue::putInt(head.next,(unsigned char*)getBuffer() + headOffset);
		SMDValue::putShort(head.space,(unsigned char*)getBuffer() + sizeof(int) + headOffset);
		SMDValue::putShort(head.reserve1,(unsigned char*)getBuffer() + sizeof(int) + sizeof(short) + headOffset);
		SMDValue::putShort(head.reserve1,(unsigned char*)getBuffer() + sizeof(int) + sizeof(short) + sizeof(int) + headOffset);
		return true;
	}

	// ��ȡ���ӵ���һ��ҳ
	unsigned int getNextPageNo() const
	{
		return head.next;
	}

	// �Ƿ������һ��ҳ
	bool isLastPage() const
	{
		return head.next == 0xFFFFFFFF;
	}

	// �������ӵ���һ��ҳ
	void setNextPageNo(unsigned int nextPageNo)
	{
		this->head.next = nextPageNo;
		modify = true;
	}

	//��ȡpageno
	int getPageNo()
	{
		return pageno;
	}

	// ��ȡҳ�ڴ�ռ��ַ
	const char* getBuffer() const
	{
		return data.data();
	}
	int getOffsetToEndSize(int offset)
	{
		offset %= getSize();
		return getSize() - offset;
	}
	// ��ȡҳ��С
	unsigned short getSize() const
	{
		return size;
	}

	// ��ȡҳ���ļ���λ��
	unsigned int getFileOffset() const
	{
		return pageno * size;
	}

	// ��ȡ��ͷ��ĵ�ַ
	unsigned int getContentOffset() const
	{
		if(pageno == 0)
		{
			return pageno * size + sizeof(head) + STROAGE_FLAG_LENGTH;;
		}
		else
		{
			return pageno * size + sizeof(head);
		}
	}

	// ��ȡҳ��ʣ��ռ�λ��
	unsigned short getSpaceOffset() const
	{
		return size - head.space;
	}

	// ��ȡҳ��ʣ��ռ�
	unsigned short getSpace() const
	{
		return head.space;
	}

	// ��Page�ڶ�ȡ�ڴ�
	int read(unsigned int offset,char* buffer,unsigned short sz) const
	{
		unsigned int pageOffset = offset % size;
		if(pageOffset + sz > getSpaceOffset())
		{
			sz = getSpaceOffset() - pageOffset;
		}

		memcpy(buffer,getBuffer() + pageOffset,sz);
		return sz;
	}

	// �洢�ڴ浽Page��
	int write(const char* buffer,unsigned short sz)
	{
		if(head.space == 0)
			return 0;

		int wr = std::min(head.space,sz);
		memcpy((char*)getBuffer() + getSpaceOffset(),buffer,wr);
		head.space -= wr;

		modify = true;
		return wr;
	}

	// �洢�ڴ浽Page��
	int write(const char* buffer,unsigned short sz,unsigned short offset)
	{
		offset %= getSize();

		assert(size - offset >= sz);

		memcpy((char*)getBuffer() + offset,buffer,sz);
		modify = true;
		return sz;
	}
private:
	// �ж��Ƿ��ǵ�0��ҳ,���⴦���ļ���ʶ
	void checkFirstPage()
	{
		if(pageno == 0)
		{
			strcpy((char*)data.data(),STORAGE_FLAG);
			this->head.space = size - sizeof(head) - STROAGE_FLAG_LENGTH;
		}
	}
private:
	// �Ƿ����޸�
	bool modify;

	// ҳ�ռ��С
	unsigned short size;

	// ҳ����
	unsigned int pageno;

	// �ڴ�����
	std::string data;
	
	// ҳ�ڲ��ṹ,��Ҫ�洢���ļ���
	SMDPageHead head;
};

}

#endif
