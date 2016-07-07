#ifndef _VARINT_FIELD_H_
#define _VARINT_FIELD_H_

#include "NumberType.h"
#include <vector>

namespace kvdbEngine
{

typedef unsigned char byte;
class VarIntField :
	public NumberType
{
public:
	VarIntField(string name);
	~VarIntField(void);
};

}
#endif

