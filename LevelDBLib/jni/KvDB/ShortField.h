#ifndef _SHORT_FIELD_H_
#define _SHORT_FIELD_H_

#include "NumberType.h"

namespace kvdbEngine
{
	class ShortField :
		public NumberType
	{
	public:
		ShortField(const string &name);
		~ShortField(void);
	};
}

#endif


