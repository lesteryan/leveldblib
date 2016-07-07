#ifndef _VAR_TEXT_FIELD_H_ 
#define _VAR_TEXT_FIELD_H_ 

#include "TextType.h"
namespace kvdbEngine
{

class VarTextField :
	public TextType
{
public:
	VarTextField(const string &name);
	~VarTextField(void);
};

}

#endif




