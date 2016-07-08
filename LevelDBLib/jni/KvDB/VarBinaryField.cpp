#include "VarBinaryField.h"

namespace kvdbEngine
{

VarBinaryField::VarBinaryField(string name)
{
	init(name,FIELD_TYPE_VAR_BINARY,name);
	length = 0;
}

VarBinaryField::~VarBinaryField(void)
{
}
}
