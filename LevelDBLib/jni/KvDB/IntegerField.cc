#include "IntegerField.h"

namespace kvdbEngine
{

IntegerField::IntegerField(const string & name)
{
	init(name,FIELD_TYPE_INT,name);
}


IntegerField::~IntegerField(void)
{
}

}
