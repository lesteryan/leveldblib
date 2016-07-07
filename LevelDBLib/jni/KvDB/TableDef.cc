#include "TableDef.h"
#include "FieldTypeFactory.h"

namespace kvdbEngine
{
	TableDef::TableDef(string name,bool IsSaveByGroup,SMD_INDEX_INDEXTYPE enumIndexType)
	{
		setTableName(name);
		this->IsSaveByGroup = IsSaveByGroup;
		m_enumIndexType = enumIndexType;
	}

	TableDef::~TableDef(void)
	{
		Release();
	}

	bool TableDef::setTableName(const std::string &name)
	{
		tableName = name;
		if (tableName.empty())
		{
			return false;
		}
		return true;
	}

	string TableDef::getTableName()
	{
		return tableName;
	}
	
	SMD_INDEX_INDEXTYPE TableDef::getIndexType()
	{
		return m_enumIndexType;
	}

	bool TableDef::add( FieldType * field )
	{
		if (field)
		{
			FieldSet.push_back(field);
			return true;
		}
		return false;
	}

	FieldType * TableDef::getField(const std::string & name)
	{
		vector<FieldType *>::iterator iter;  
		for (iter=FieldSet.begin();iter!=FieldSet.end();++iter)  
		{  
			string fieldName = (*iter)->getName();

			if (0 == fieldName.compare(name))
				return *iter;
		}  
		return NULL;
	}

	FieldType * TableDef::getField(int index)
	{
		if (IsOverFlow(index))
		{
			return NULL;
		}
		FieldType * field = FieldSet[index];
		return field;
	}


	Enum_FieldType TableDef::getFieldType( const std::string &name )
	{
		if (getField(name)!=NULL)
		{
			return (getField(name))->getType();
		}
		return FIELD_TYPE_UNKNOWN;
	}

	Enum_FieldType TableDef::getFieldType( int index )
	{
		if (getField(index)!=NULL)
		{
			return (getField(index))->getType();
		}
		return FIELD_TYPE_UNKNOWN;
	}

	bool TableDef::IsOverFlow( int iIndex )
	{
		if (iIndex < 0 || iIndex >= FieldSet.size())
		{
			return true;
		}
		return false;
	}

	bool TableDef::write( SMDStream& stream )
	{
		stream.writeString(tableName);
		stream.writeShort(m_enumIndexType);
		stream.writeShort(IsSaveByGroup);
		int nRSize = FieldSet.size();
		//标记为写入
		stream.writeInt(nRSize);
		//一次写入SMDStream
		for (int i=0;i<nRSize;i++)
		{
			FieldType * p = FieldSet[i];
			p->write(stream);
		}
		return true;
	}

	bool TableDef::read( SMDStream& stream )
	{
		if (!stream.size())
		{
			return false;
		}

		Release();
		stream.readString(tableName);
		short nIndexType;
		stream.readShort(nIndexType);
		getIndexTypeFromShort(m_enumIndexType,nIndexType);
		short IsSByGroup;
		stream.readShort(IsSByGroup);
		this->IsSaveByGroup = IsSByGroup;

		int nRSize;
		stream.readInt(nRSize);
		for (int i=0;i<nRSize;i++)
		{
//			FieldType *p = new FieldType();
			FieldType *p = FieldTypeFactory::createFieldByStream(stream);
			FieldSet.push_back(p);
		}
		return true;
	}

	void TableDef::Release()
	{
		vector<FieldType * >::iterator iter;  
		for (iter=FieldSet.begin();iter!=FieldSet.end();++iter)  
		{  
			if(*iter)
            {
                delete *iter;
                *iter = NULL;
            }
		}
		FieldSet.clear();
	}

	void TableDef::getIndexTypeFromShort(SMD_INDEX_INDEXTYPE& enumIndexType,short nIndexType)
	{
		switch (nIndexType)
		{
		case 0:
			enumIndexType = SMD_INDEX_INDEXTYPE_NULL;
			break;
		case 1:
			enumIndexType = SMD_INDEX_INDEXTYPE_BTREE;
			break;
		case 2:
			enumIndexType = SMD_INDEX_INDEXTYPE_BPTREE;
			break;
		case 3:
			enumIndexType = SMD_INDEX_INDEXTYPE_HASH;
			break;
		default:
			enumIndexType = SMD_INDEX_INDEXTYPE_NULL;
			break;
		}
	}
}
