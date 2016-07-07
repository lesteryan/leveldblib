#ifndef _SMD_PAGE_H_
#define _SMD_PAGE_H_

#include "SMDTableConfig.h"
#include "SMDValue.h"

namespace kvdbEngine
{

typedef struct
{
	// 文件页剩余空间
	unsigned short space;

	// 保留两个字节
	unsigned short reserve1;

	// 保留两个字节
	unsigned int reserve2;

	// 下一个文件页编号
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

	// 释放页空间
	void freeSpace()
	{
		this->modify = true;
		head.space = size - sizeof(head);

		memset((char*)data.data(),0,data.size());

		checkFirstPage();
	}

	// 是否有修改
	bool isModify() const
	{
		return modify;
	}

	// 设置Modify
	void setModify(bool modify)
	{
		this->modify = modify;
	}

	// 从内存中恢复文件结构
	// 读取页头信息
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

	// 存储文件结构到内存中
	// 保存页头信息
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

	// 获取连接的下一个页
	unsigned int getNextPageNo() const
	{
		return head.next;
	}

	// 是否是最后一个页
	bool isLastPage() const
	{
		return head.next == 0xFFFFFFFF;
	}

	// 设置连接的下一个页
	void setNextPageNo(unsigned int nextPageNo)
	{
		this->head.next = nextPageNo;
		modify = true;
	}

	//获取pageno
	int getPageNo()
	{
		return pageno;
	}

	// 获取页内存空间地址
	const char* getBuffer() const
	{
		return data.data();
	}
	int getOffsetToEndSize(int offset)
	{
		offset %= getSize();
		return getSize() - offset;
	}
	// 获取页大小
	unsigned short getSize() const
	{
		return size;
	}

	// 获取页在文件内位置
	unsigned int getFileOffset() const
	{
		return pageno * size;
	}

	// 获取除头外的地址
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

	// 获取页内剩余空间位置
	unsigned short getSpaceOffset() const
	{
		return size - head.space;
	}

	// 获取页内剩余空间
	unsigned short getSpace() const
	{
		return head.space;
	}

	// 从Page内读取内存
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

	// 存储内存到Page中
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

	// 存储内存到Page中
	int write(const char* buffer,unsigned short sz,unsigned short offset)
	{
		offset %= getSize();

		assert(size - offset >= sz);

		memcpy((char*)getBuffer() + offset,buffer,sz);
		modify = true;
		return sz;
	}
private:
	// 判断是否是第0个页,特殊处理文件标识
	void checkFirstPage()
	{
		if(pageno == 0)
		{
			strcpy((char*)data.data(),STORAGE_FLAG);
			this->head.space = size - sizeof(head) - STROAGE_FLAG_LENGTH;
		}
	}
private:
	// 是否有修改
	bool modify;

	// 页空间大小
	unsigned short size;

	// 页面编号
	unsigned int pageno;

	// 内存数据
	std::string data;
	
	// 页内部结构,需要存储到文件内
	SMDPageHead head;
};

}

#endif
