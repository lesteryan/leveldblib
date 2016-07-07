#ifndef _TEXT_FIELD_H_
#define _TEXT_FIELD_H_


#include "TextType.h"

namespace kvdbEngine
{

class TextField :
	public TextType
{
public:
	TextField(string name,unsigned int length);
	~TextField(void);
protected:
};

}

#endif

