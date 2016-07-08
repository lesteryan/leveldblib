#include "VarTextField.h"

namespace kvdbEngine
{

	VarTextField::VarTextField(const string & name)
	{
		init(name,FIELD_TYPE_VAR_TEXT,name);
		length = 0;
	}


	VarTextField::~VarTextField(void)
	{
		
	}


}
