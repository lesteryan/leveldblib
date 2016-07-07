#ifndef _TEXT_TYPE_H_
#define _TEXT_TYPE_H_

#include "FieldType.h"
namespace kvdbEngine
{

class TextType :
	public FieldType
{
public:
	TextType(void);
	virtual ~TextType(void);

	//设置长度
	//@length 初始化时不设定长度则将变为变长的，如果设定长度值，则为定长的
	bool setLength(unsigned int unlength);

	//获取长度
	unsigned int getLength();

	//save 
	virtual bool save(SMDStream& stream);

	//read
	virtual bool readArr(SMDStream & stream);
protected: 
	//char * buffer;
	int length;

};

}

#endif
