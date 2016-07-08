#include "SMDIndexSegment.h"

namespace kvdbEngine
{
bool SMDIndexSegment::saveAllIndex()
{
	bool bRet = true;

	std::map<std::string, tIndexTableInfo>::iterator iterM = m_mapIndexTableInfo.begin();
	for (; iterM != m_mapIndexTableInfo.end(); ++iterM)
	{
		iterM->second.enumKeyType = iterM->second.pIndexBase->getIndexKeyType();

		if (!iterM->second.pIndexBase->saveIndex(iterM->second.nOffset))
		{
			bRet = false;
		}
		delete iterM->second.pIndexBase;
		iterM->second.pIndexBase = NULL;
	}

	return bRet;
}

void SMDIndexSegment::clearAll()
{
	if (m_mapIndexTableInfo.empty())
	{
		return;
	}
	
	std::map<std::string, tIndexTableInfo>::iterator iterM = m_mapIndexTableInfo.begin();
	for (; iterM != m_mapIndexTableInfo.end(); ++iterM)
	{
		if (iterM->second.pIndexBase != NULL)
		{
			delete iterM->second.pIndexBase;
			iterM->second.pIndexBase = NULL;
		}
	}

	m_mapIndexTableInfo.clear();
}

bool SMDIndexSegment::save()
{
	if (!saveAllIndex()) { return false; }
	
	if(m_mapIndexTableInfo.empty()) { return true; }

	SMDPageFile* file = m_pageMgr->createPageFile(SEG_TYPE_INDEX,SMD_INDEX_NAME,0);
	if (!file) { return false; }
	file->clearPage();

	SMDMemStream stream;
	if(!stream.writeInt(m_mapIndexTableInfo.size())) {	return false; }

	std::map<std::string, tIndexTableInfo>::iterator iterI=m_mapIndexTableInfo.begin();
	for(; iterI!=m_mapIndexTableInfo.end(); ++iterI)
	{
		stream.writeString(iterI->first);
		stream.writeUInt(iterI->second.nOffset);
		stream.writeShort(iterI->second.enumKeyType);
	}

	file->put(stream);
	delete file;
	file = NULL;

	return true;
}

bool SMDIndexSegment::read()
{
	m_mapIndexTableInfo.clear();

	SMDPageFile* pPageFile = m_pageMgr->getPageFile(SEG_TYPE_INDEX,SMD_INDEX_NAME);
	if (!pPageFile) { return false; }

	int nSize;
	pPageFile->setToFristPageFile();
	SMDMemStream stream;
	pPageFile->get(stream,pPageFile->getPageFileBeginOffset());
	if(!stream.readInt(nSize)) { return false; }

	for(int i=0;i<nSize;++i)
	{
		string name;
		tIndexTableInfo tTblInfo;
		short nIndexType;
		short nKeyType;
		stream.readString(name);
		stream.readUInt(tTblInfo.nOffset);
		stream.readShort(nKeyType);
		getKeyTypeFromShort(tTblInfo.enumKeyType,nKeyType);
		m_mapIndexTableInfo.insert(pair<string,tIndexTableInfo>(name,tTblInfo));
	}

	delete pPageFile;
	return true;
}

void SMDIndexSegment::getKeyTypeFromShort(SMD_INDEX_KEYTYPE& enumKeyType,short nKeyType)
{
	switch (nKeyType)
	{
	case 0:
		enumKeyType = SMD_INDEX_KEYTYPE_NULL;
		break;
	case 1:
		enumKeyType = SMD_INDEX_KEYTYPE_INT;
		break;
	case 2:
		enumKeyType = SMD_INDEX_KEYTYPE_UINT;
		break;
	case 3:
		enumKeyType = SMD_INDEX_KEYTYPE_DOUBLE;
		break;
	case 4:
		enumKeyType = SMD_INDEX_KEYTYPE_STRING;
		break;
	case 5:
		enumKeyType = SMD_INDEX_KEYTYPE_PAIR;
		break;
	default:
		enumKeyType = SMD_INDEX_KEYTYPE_NULL;
		break;
	}
}

bool isIndexSupport(SMD_INDEX_INDEXTYPE enumIndexType)
{
	if (enumIndexType == SMD_INDEX_INDEXTYPE_BTREE ||
		enumIndexType == SMD_INDEX_INDEXTYPE_BPTREE)
	{
		return true;
	}
	else
	{
		return false;
	}
}

}
