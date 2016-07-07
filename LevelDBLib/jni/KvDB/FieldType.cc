#include "FieldType.h"

namespace kvdbEngine
{

FieldType::FieldType()
{
	DataType = FIELD_TYPE_UNKNOWN;
	ReadOnly = false;
	IsAllowNULL = true;
	IsHaveHead = false;
	//IsRelative = true;
}


FieldType::~FieldType(void)
{
	ReleaseHead();
}

//获取字段别名
string FieldType::getAlias()
{
	return Alias;
}

//设置字段别名
bool FieldType::setAlias(string alias)
{
		Alias.resize(alias.size());
		Alias = alias;
		return true;
}

//获取只读属性
bool FieldType::getIsReadOnly()
{
	return ReadOnly;
}

//设置只读属性
bool FieldType::setReadOnly(bool IsReadOnly)
{
	ReadOnly = IsReadOnly;
	return true;
}

//获取字段类型
Enum_FieldType FieldType::getType()
{
	return DataType;
}

//设置字段类型
bool FieldType::setFieldType(Enum_FieldType typeCode)
{
	DataType = typeCode;
	if ( DataType >=1 && DataType <= 7 )
	{
		return true;
	}
	else
		return false;
}

//获取字段名称
string FieldType::getName()
{
	return FieldName;
}

//设置字段名称
bool FieldType::setName(string name)
{
	FieldName.resize(name.size());
	FieldName = name;
	if ( FieldName.empty() )
	{
		return false;
	}
	else
		return true;
}

//获取字段是否可为空
bool FieldType::getIsAllowNULL()
{
	return IsAllowNULL;
}

//设置字段是否为空属性
bool FieldType::setALlowNULL( bool IsNull)
{
	IsAllowNULL = IsNull;
	return true;
}

bool FieldType::write( SMDStream& stream )
{
	//标记为写入
	short type = this->getType();
	stream.writeShort(type);
	stream.writeString(this->getName());
	stream.writeString(this->getAlias());
	stream.writeShort(this->getIsReadOnly());
	stream.writeShort(this->getIsAllowNULL());
	stream.writeShort(this->getIsHaveHead());
	//写head
	if (this->getIsHaveHead())
	{
		writeFieldHead(stream);
	}

	writeExternFields(stream);
	return this->save(stream);
}

bool FieldType::read( SMDStream& stream )
{
	if (!stream.size())
	{
		return false;
	}
	short IsReadOnly;
	short IsALLowNULL;
	short HaveHead;
	string Name ="";
	string alias = "";
	stream.readString(Name);
	stream.readString(alias);
	stream.readShort(IsReadOnly);
	stream.readShort(IsALLowNULL);
	stream.readShort(HaveHead);
	this->setName(Name);
	this->setAlias(Alias);
	this->setReadOnly(IsReadOnly);
	this->setALlowNULL(IsALLowNULL);
	this->IsHaveHead = HaveHead;
	if (this->IsHaveHead)
	{
		readFieldHead(stream);
	}	

	readExternFields(stream);
	return readArr(stream);
}

bool FieldType::readExternFields(SMDStream& stream)
{
	int size = 0;
	stream.readInt(size);

	for(int i=0;i<size;i++)
	{
		std::string key;
		std::string val;
		stream.readString(key);
		stream.readString(val);

		externFields.insert(std::pair<std::string,std::string>(key,val));
	}

	return true;
}

bool FieldType::writeExternFields(SMDStream& stream)
{
	stream.writeInt(externFields.size());
	if(!externFields.empty())
	{
		for(std::map<std::string,std::string>::iterator it = externFields.begin();it != externFields.end();++it)
		{
			stream.writeString(it->first);
			stream.writeString(it->second);
		}
	}

	return true;
}

bool FieldType::save( SMDStream& stream )
{
	return true;
}

bool FieldType::readArr( SMDStream & stream )
{
	return true;
}
bool FieldType::getIsHaveHead()
{
	return this->IsHaveHead;
}

bool FieldType::init( const string &name, Enum_FieldType Datatype,const string & Alias,
	bool IsHaveHead /*= false*/,bool ReadOnly /*= false*/, bool IsAllowNULL /*= true*/ )
{
	setName(name);
	setFieldType(Datatype);
	setAlias(Alias);
	setReadOnly(ReadOnly);
	setALlowNULL(IsAllowNULL);
	return true;
}

bool FieldType::setFieldHead( FieldHead & fhead )
{
	this->IsHaveHead = true;
	this->fhead.headtype = fhead.headtype;

	switch (this->fhead.headtype)
	{
	case HEAD_TYPE_UNKNOWN:
		return false;
		break;
	case HEAD_TYPE_DOUBLE:
		this->fhead.headValue.head_double = fhead.headValue.head_double;
		return true;
		break;
	case HEAD_TYPE_INT:
		this->fhead.headValue.head_int = fhead.headValue.head_int;
		return true;
		break;
	case HEAD_TYPE_STRING_BINARY:
		this->fhead.headValue.head_str.nLength = fhead.headValue.head_str.nLength;
		memcpy(this->fhead.headValue.head_str.content,fhead.headValue.head_str.content, fhead.headValue.head_str.nLength);
		return false;
		break;
	}
	return true;
}

bool FieldType::writeFieldHead(SMDStream & stream)
{
	short type = this->fhead.headtype;
	stream.writeShort(type);
	switch (this->fhead.headtype)
	{
	case HEAD_TYPE_DOUBLE:
		stream.writeDouble(this->fhead.headValue.head_double);
		return true;
		break;
	case HEAD_TYPE_INT:
		stream.writeInt(this->fhead.headValue.head_int);
		return true;
		break;
	case HEAD_TYPE_STRING_BINARY:
		stream.writeInt(this->fhead.headValue.head_str.nLength );
		stream.writeBytes(this->fhead.headValue.head_str.content,this->fhead.headValue.head_str.nLength);
		return false;
		break;
	}
	return true;
}

bool FieldType::readFieldHead( SMDStream & stream )
{
	short type;
	stream.readShort(type);
	this->fhead.headtype = (Enum_HeadType)type;
	switch (this->fhead.headtype)
	{
	case HEAD_TYPE_DOUBLE:
		stream.readDouble(this->fhead.headValue.head_double);
		return true;
		break;
	case HEAD_TYPE_INT:
		stream.readInt(this->fhead.headValue.head_int);
		return true;
		break;
	case HEAD_TYPE_STRING_BINARY:
		if (this->fhead.headValue.head_str.content != NULL)
		{
			free(this->fhead.headValue.head_str.content);
		}
		stream.readInt(this->fhead.headValue.head_str.nLength );
		this->fhead.headValue.head_str.content = (char *)malloc(this->fhead.headValue.head_str.nLength);
		stream.readBytes(this->fhead.headValue.head_str.content,this->fhead.headValue.head_str.nLength);
		return false;
		break;
	}
	return true;
}

FieldHead * FieldType::getFieldHead()
{
	return &(this->fhead);
}

}
