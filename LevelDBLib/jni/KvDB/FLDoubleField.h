#ifndef _FD_DOUBLE_FIELD_H_
#define _FD_DOUBLE_FIELD_H_

#include "FDNumberType.h"
namespace kvdbEngine
{
	class FLDoubleField :
		public FDNumberType
	{
	public:
		FLDoubleField(const string &name, unsigned short scale);
		~FLDoubleField(void);
	};
}

#endif

