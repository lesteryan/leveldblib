#ifndef _DOUBLE_FIELD_H_
#define _DOUBLE_FIELD_H_

#include "FDNumberType.h"

namespace kvdbEngine
{
	class DoubleField :
		public FDNumberType
	{
	public:
		DoubleField(const string &name);
		~DoubleField(void);
	};
}


#endif

