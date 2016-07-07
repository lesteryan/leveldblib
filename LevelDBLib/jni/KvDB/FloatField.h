#ifndef _FLOAT_FIELD_H_
#define _FLOAT_FIELD_H_
#include "FDNumberType.h"

namespace kvdbEngine
{
	class FloatField :
		public FDNumberType
	{
	public:
		FloatField(const string &name);
		~FloatField(void);
	};

}

#endif 


