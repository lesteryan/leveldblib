#include "LongField.h"

namespace kvdbEngine
{

LongField::LongField(const string & name)
{
	init(name,FIELD_TYPE_LONG,name);
}


LongField::~LongField(void)
{
}
}
