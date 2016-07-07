#include "TextField.h"

namespace kvdbEngine
{

TextField::TextField(string name,unsigned int length)
{
	init(name,FIELD_TYPE_TEXT,name);
	setLength(length);
}
TextField::~TextField(void)
{
}


}
