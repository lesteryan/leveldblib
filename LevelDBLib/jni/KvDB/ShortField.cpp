#include "ShortField.h"

namespace kvdbEngine
{
	
ShortField::ShortField(const string & name)
{
	init(name,FIELD_TYPE_SHORT,name);
}


ShortField::~ShortField(void)
{
}


}
