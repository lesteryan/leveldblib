#ifndef _LONG_FIELD_H_
#define _LONG_FIELD_H_

#include "NumberType.h"

namespace kvdbEngine
{

class LongField :
	public NumberType
{
public:
	LongField(const string &name);
	~LongField(void);

};
}
#endif


