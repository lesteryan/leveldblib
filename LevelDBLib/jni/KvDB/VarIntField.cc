#include "VarIntField.h"

namespace kvdbEngine
{

VarIntField::VarIntField(string name)
{
	init(name,FIELD_TYPE_VAR_INT,name);
}


VarIntField::~VarIntField(void)
{
}

}
