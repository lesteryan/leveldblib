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

	//���ó���
	//@length ��ʼ��ʱ���趨�����򽫱�Ϊ�䳤�ģ�����趨����ֵ����Ϊ������
	bool setLength(unsigned int unlength);

	//��ȡ����
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
