#ifndef _FDNUMBER_TYPE_H_
#define _FDNUMBER_TYPE_H_

#include "FieldType.h"
namespace kvdbEngine
{

class FDNumberType :
	public FieldType
{
public:
	FDNumberType(void);
	virtual ~FDNumberType(void);

	//����С��λ
	//@precision
	bool setScale(short scale);

	//��ȡС��λ
	short getScale();
	//save 
	virtual bool save(SMDStream& stream);

	//read
	virtual bool readArr(SMDStream & stream);
protected:

	short scale;
};

}
#endif


