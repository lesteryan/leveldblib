#ifndef _FIELD_TYPE_FACTORY_H_ 
#define _FIELD_TYPE_FACTORY_H_ 
#include "FieldType.h"
#include "TextField.h"
#include "VarTextField.h"
#include "BinaryField.h"
#include "VarBinaryField.h"
#include "ShortField.h"
#include "IntegerField.h"
#include "LongField.h"
#include "FloatField.h"
#include "DoubleField.h"
#include "FLDoubleField.h"
#include "VarIntField.h"

namespace kvdbEngine
{

class FieldTypeFactory
{
public:

	static FieldType * createField(Enum_FieldType type,const string &name)
	{
		switch(type)
		{
		case FIELD_TYPE_TEXT:
			return new TextField(name,20);
		case FIELD_TYPE_VAR_TEXT:
			return new VarTextField(name);
		case FIELD_TYPE_BINARY:
			return new BinaryField(name,20);
		case FIELD_TYPE_VAR_BINARY:
			return new VarBinaryField(name);
		case FIELD_TYPE_FLOAT:
			return new FloatField(name);
		case FIELD_TYPE_DOUBLE:
			return new DoubleField(name);
		case FIELD_TYPE_FLDOUBLE:
			return new FLDoubleField(name,2);
		case FIELD_TYPE_VAR_INT:
			return new VarIntField(name);
		case FIELD_TYPE_SHORT:
			return new ShortField(name);
		case FIELD_TYPE_INT:
			return new IntegerField(name);
		case FIELD_TYPE_LONG:
			return new LongField(name);
		}
		return NULL;
	}

	static FieldType * createFieldByStream(SMDStream &stream)
	{
		short type;
		stream.readShort(type);
		switch(type)
		{
		case FIELD_TYPE_TEXT:
		case FIELD_TYPE_VAR_TEXT:
		case FIELD_TYPE_BINARY:
		case FIELD_TYPE_VAR_BINARY:
			{
				TextType * vartext =(TextType *)FieldTypeFactory::createField((Enum_FieldType)type,"");
				vartext->read(stream);
				return vartext;
			}
			break;
		case FIELD_TYPE_FLOAT:
		case FIELD_TYPE_DOUBLE:
		case FIELD_TYPE_FLDOUBLE:
			{
				FDNumberType *fdField = (FDNumberType *)FieldTypeFactory::createField((Enum_FieldType)type,"");
				fdField->read(stream);
				return fdField;
			}
			break;
		case FIELD_TYPE_VAR_INT:
		case FIELD_TYPE_SHORT:
		case FIELD_TYPE_INT:
		case FIELD_TYPE_LONG:
			{
				NumberType *numField = (NumberType *)FieldTypeFactory::createField((Enum_FieldType)type,"");
				numField->read(stream);
				return numField;
			}
			break;
		}
		return NULL;
	}
};

}

#endif



