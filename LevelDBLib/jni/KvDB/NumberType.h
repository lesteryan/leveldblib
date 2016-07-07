#ifndef _NUMBER_TYPE_H_
#define _NUMBER_TYPE_H_

#include "FieldType.h"
namespace kvdbEngine
{

class NumberType :
	public FieldType
{
public:
	NumberType(void);
	virtual ~NumberType(void);

};

}

#endif
