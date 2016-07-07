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

	//设置小数位
	//@precision
	bool setScale(short scale);

	//获取小数位
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


