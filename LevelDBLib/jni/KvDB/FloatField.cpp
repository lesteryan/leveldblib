#include "FloatField.h"

namespace kvdbEngine
{

FloatField::FloatField(const string & name)
{
	init(name,FIELD_TYPE_FLOAT,name);
	//setScale(24);
}

FloatField::~FloatField(void)
{
}
}
