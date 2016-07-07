#ifndef _INTEGER_FIELD_H_
#define _INTEGER_FIELD_H_

#include "NumberType.h"
namespace kvdbEngine
{
	class IntegerField :
		public NumberType
	{
	public:
		IntegerField(const string &name);
		~IntegerField(void);
	};
}

#endif

