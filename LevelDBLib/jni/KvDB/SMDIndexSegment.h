#ifndef _SMD_INDEX_SEGMENT_H_
#define _SMD_INDEX_SEGMENT_H_

#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <functional>
#include <iostream>
#include <sstream>
#include <typeinfo>
using namespace std;

#include <math.h>
#include "SMDSegment.h"
#include "SMDPageFile.h"
#include "SMDPageManagement.h"

namespace kvdbEngine
{
#define SMD_INDEX_NAME "smd_index"
#define SMD_INVALIDINDEX_RETURN 0xFFFFFFFF
#define SMD_PAGEHEADER_SIZE 16
//index support type
typedef enum _enumKeyType
{
	SMD_INDEX_KEYTYPE_NULL		= 0,
	SMD_INDEX_KEYTYPE_INT		= 1,		//int
	SMD_INDEX_KEYTYPE_UINT		= 2,		//unsigned int
	SMD_INDEX_KEYTYPE_DOUBLE	= 3,		//double
	SMD_INDEX_KEYTYPE_STRING	= 4,		//std::string
	SMD_INDEX_KEYTYPE_PAIR		= 5,		//tPairKey_t
}SMD_INDEX_KEYTYPE;

typedef enum _enumIndexType
{
	SMD_INDEX_INDEXTYPE_NULL		= 0,	//default is null
	SMD_INDEX_INDEXTYPE_BTREE		= 1,	//B tree
	SMD_INDEX_INDEXTYPE_BPTREE		= 2,	//B+ tree
	SMD_INDEX_INDEXTYPE_HASH		= 3,	//not implemented
}SMD_INDEX_INDEXTYPE;

class SMDIndexSegment;
class SMDIndexBase;
template<class SEARCH_KEY, class FILE_KEY> class SMDIndex;
template<class SEARCH_KEY, class FILE_KEY> class SMDIndexBp;
//for build btree with pair keys: keyStart and keyEnd
template<class ANY_KEY>
struct PairKey_t
{
	ANY_KEY nKeyS;
	ANY_KEY nKeyE;

	PairKey_t() : nKeyS(0),nKeyE(0)
	{}
	PairKey_t(ANY_KEY key)
	{
		this->nKeyS = key;
		this->nKeyE = key;
	}
	PairKey_t(const PairKey_t<ANY_KEY>& pairAny)
	{
		this->nKeyS = pairAny.nKeyS;
		this->nKeyE = pairAny.nKeyE;
	}

	PairKey_t(ANY_KEY keyS,ANY_KEY keyE)
	{
		this->nKeyS = keyS;
		this->nKeyE = keyE;
	}
	PairKey_t& operator=(ANY_KEY keyS)
	{
		this->nKeyS = keyS;
		this->nKeyE = keyS;

		return *this;
	}
	PairKey_t& operator=(const PairKey_t<ANY_KEY>& pairAny)
	{
		this->nKeyS = pairAny.nKeyS;
		this->nKeyE = pairAny.nKeyE;

		return *this;
	}
	bool operator<(const PairKey_t& tTwo) const
	{
		return nKeyS < tTwo.nKeyS && nKeyE < tTwo.nKeyE;
	}

	bool operator>(const PairKey_t& tTwo) const
	{
		return nKeyS > tTwo.nKeyS && nKeyE > tTwo.nKeyE;
	}

	//friend ostream& operator<<(ostream& os,const _SKey_t& FILE_KEY);
};

// << and >> overload function for SMDStream class to deal with PairKey_t
template<class ANY_KEY>
SMDStream& operator<<(SMDStream& stream,PairKey_t<ANY_KEY>& pairKey)
{
	stream << pairKey.nKeyS;
	stream << pairKey.nKeyE;

	return stream;
}
template<class ANY_KEY>
SMDStream& operator>>(SMDStream& stream,PairKey_t<ANY_KEY>& pairKey)
{
	stream >> pairKey.nKeyS;
	stream >> pairKey.nKeyE;

	return stream;
}
// index table root offset management
class SMDIndexSegment : public SMDSegment
{
public:
	SMDIndexSegment()
	{
		m_pageMgr = NULL;
		read();
	}
	SMDIndexSegment(SMDPageManagement* pageMgr)
	{
		m_pageMgr = pageMgr;
		read();
	}
	virtual ~SMDIndexSegment()
	{
		clearAll();
	}
	
public:
	//for SMDSStorage
	bool save();
	bool read();
	//do clean work
	void clearAll();
	//save all index to file when insert all record, this will delete all in memory
	bool saveAllIndex();
	//insert function for index segment
	template<class SEARCH_KEY, class FILE_KEY>
	bool insertKeyOffset(const string& sName, const FILE_KEY& key, unsigned int nOffset, 
		SMD_INDEX_INDEXTYPE enumIndexType);
	//search from memory before saveAllIndex
	template<class SEARCH_KEY, class FILE_KEY>
	unsigned int searchFromMem(const string& sName, const SEARCH_KEY& Key);
	//search from file
	template<class SEARCH_KEY>
	unsigned int searchFromFile(const string& sName, const SEARCH_KEY& key, SMD_INDEX_INDEXTYPE enumIndexType);
	virtual SegType getType() {	return SEG_TYPE_INDEX;}
	bool isIndexSupport(SMD_INDEX_INDEXTYPE enumIndexType);
	//template<class SEARCH_KEY, class FILE_KEY> friend class SMDIndex;
	
private:
	typedef struct _tIndexTableInfo
	{
		SMDIndexBase* pIndexBase;			//to hold the pointer of active index in memory
		unsigned int nOffset;				//to record the index's offset of one table
		SMD_INDEX_INDEXTYPE enumIndexType;	//to record witch type of index do the table hold
		SMD_INDEX_KEYTYPE enumKeyType;		//to record index's key type

		_tIndexTableInfo()
		{
			pIndexBase = NULL;
			nOffset = SMD_INVALIDINDEX_RETURN;
			enumIndexType = SMD_INDEX_INDEXTYPE_NULL;
			enumKeyType = SMD_INDEX_KEYTYPE_NULL;
		}
		_tIndexTableInfo(SMDIndexBase* pIndexBase,unsigned int nOffset,
			SMD_INDEX_KEYTYPE enumKeyType,SMD_INDEX_INDEXTYPE enumIndexType)
		{
			this->pIndexBase = pIndexBase;
			this->nOffset = nOffset;
			this->enumKeyType = enumKeyType;
			this->enumIndexType = enumIndexType;
		}
	}tIndexTableInfo;

	//all index table info management
	std::map<std::string, tIndexTableInfo> m_mapIndexTableInfo;
	//get index table info
	SMDPageManagement* m_pageMgr;
private:
	void getKeyTypeFromShort(SMD_INDEX_KEYTYPE& enumKeyType,short nKeyType);
	template<class SEARCH_KEY,class FILE_KEY>
	bool insertKeyOffsetBTree(const string& sName, const FILE_KEY& key, unsigned int nOffset);
	template<class SEARCH_KEY,class FILE_KEY>
	bool insertKeyOffsetBPTree(const string& sName, const FILE_KEY& key, unsigned int nOffset);
	template<class SEARCH_KEY,class FILE_KEY>
	unsigned int searchFromMemBTree(const string& sName, const SEARCH_KEY& Key);
	template<class SEARCH_KEY,class FILE_KEY>
	unsigned int searchFromMemBPTree(const string& sName, const SEARCH_KEY& Key);
	template<class SEARCH_KEY>
	unsigned int searchFromFileBTree(const string& sName, const SEARCH_KEY& key);
	template<class SEARCH_KEY>
	unsigned int searchFromFileBPTree(const string& sName, const SEARCH_KEY& key);
};

template<class SEARCH_KEY,class FILE_KEY>
bool SMDIndexSegment::insertKeyOffset(const string& sName,const FILE_KEY& key, unsigned int nOffset,
	SMD_INDEX_INDEXTYPE enumIndexType)
{
	if (enumIndexType == SMD_INDEX_INDEXTYPE_BTREE)
	{
		return insertKeyOffsetBTree<SEARCH_KEY,FILE_KEY>(sName,key,nOffset);
	}
	else if (enumIndexType == SMD_INDEX_INDEXTYPE_BPTREE)
	{
		return insertKeyOffsetBPTree<SEARCH_KEY,FILE_KEY>(sName,key,nOffset);
	}
	else if (enumIndexType == SMD_INDEX_INDEXTYPE_HASH)
	{	//not implemented
		return false;
	}

	return true;
}
template<class SEARCH_KEY,class FILE_KEY>
bool SMDIndexSegment::insertKeyOffsetBTree(const string& sName,const FILE_KEY& key, unsigned int nOffset)
{
	std::map<std::string, tIndexTableInfo>::iterator iterM = m_mapIndexTableInfo.find(sName);
	if (iterM == m_mapIndexTableInfo.end())
	{
		tIndexTableInfo tIdxInfo;
		SMDIndex<SEARCH_KEY, FILE_KEY>* pIndex = new SMDIndex<SEARCH_KEY, FILE_KEY>(this->m_pageMgr,sName.c_str());
		tIdxInfo.pIndexBase = (SMDIndexBase*)pIndex;
		tIdxInfo.enumIndexType = SMD_INDEX_INDEXTYPE_BTREE;
		m_mapIndexTableInfo.insert(std::pair<std::string,tIndexTableInfo>(sName,tIdxInfo));

		return pIndex->insertKeyOffset(key,nOffset);
	}
	else
	{
		SMDIndex<SEARCH_KEY, FILE_KEY>* pIndex = (SMDIndex<SEARCH_KEY, FILE_KEY>*) (iterM->second.pIndexBase);
		return pIndex->insertKeyOffset(key,nOffset);
	}
}
template<class SEARCH_KEY,class FILE_KEY>
bool SMDIndexSegment::insertKeyOffsetBPTree(const string& sName,const FILE_KEY& key, unsigned int nOffset)
{
	std::map<std::string, tIndexTableInfo>::iterator iterM = m_mapIndexTableInfo.find(sName);
	if (iterM == m_mapIndexTableInfo.end())
	{
		tIndexTableInfo tIdxInfo;
		SMDIndexBp<SEARCH_KEY, FILE_KEY>* pIndexBp = new SMDIndexBp<SEARCH_KEY, FILE_KEY>(this->m_pageMgr,sName.c_str());
		tIdxInfo.pIndexBase = (SMDIndexBase*)pIndexBp;
		tIdxInfo.enumIndexType = SMD_INDEX_INDEXTYPE_BPTREE;
		m_mapIndexTableInfo.insert(std::pair<std::string,tIndexTableInfo>(sName,tIdxInfo));

		return pIndexBp->insertKeyOffset(key,nOffset);
	}
	else
	{
		SMDIndexBp<SEARCH_KEY, FILE_KEY>* pIndexBp = (SMDIndexBp<SEARCH_KEY, FILE_KEY>*) (iterM->second.pIndexBase);
		return pIndexBp->insertKeyOffset(key,nOffset);
	}
}
template<class SEARCH_KEY,class FILE_KEY>
unsigned int SMDIndexSegment::searchFromMem(const string& sName, const SEARCH_KEY& Key)
{
	if (m_mapIndexTableInfo.empty())
	{
		return SMD_INVALIDINDEX_RETURN;
	}
	std::map<std::string, tIndexTableInfo>::iterator iterM = m_mapIndexTableInfo.find(sName);
	if (iterM != m_mapIndexTableInfo.end())
	{
		if (iterM->second.enumIndexType == SMD_INDEX_INDEXTYPE_BTREE)
		{
			SMDIndex<SEARCH_KEY, FILE_KEY>* pIndex = (SMDIndex<SEARCH_KEY, FILE_KEY>*) (iterM->second.pIndexBase);
			return pIndex->searchFromMem(Key);
		}
		else if (iterM->second.enumIndexType == SMD_INDEX_INDEXTYPE_BPTREE)
		{
			SMDIndexBp<SEARCH_KEY, FILE_KEY>* pIndexBp = (SMDIndexBp<SEARCH_KEY, FILE_KEY>*) (iterM->second.pIndexBase);
			return pIndexBp->searchFromMem(Key);
		}
		else return SMD_INVALIDINDEX_RETURN;
	}
	else
	{
		return SMD_INVALIDINDEX_RETURN;
	}
}

template<class SEARCH_KEY>
unsigned int SMDIndexSegment::searchFromFile(const string& sName,const SEARCH_KEY& key,SMD_INDEX_INDEXTYPE enumIndexType)
{
	if (m_mapIndexTableInfo.empty())
	{
		return SMD_INVALIDINDEX_RETURN;
	}
	std::map<std::string, tIndexTableInfo>::iterator iterM = m_mapIndexTableInfo.find(sName);
	if (iterM != m_mapIndexTableInfo.end())
	{
		if (enumIndexType == SMD_INDEX_INDEXTYPE_BTREE)
		{
			if (iterM->second.enumKeyType == SMD_INDEX_KEYTYPE_PAIR)
			{
				SMDIndex<SEARCH_KEY, PairKey_t<SEARCH_KEY> > indexSearch(m_pageMgr,sName.c_str());
				return indexSearch.searchFromFile(key,iterM->second.nOffset);
			}
			else //----------------------need expand...
			{
				SMDIndex<SEARCH_KEY, SEARCH_KEY> indexSearch(m_pageMgr,sName.c_str());
				return indexSearch.searchFromFile(key,iterM->second.nOffset);
			}
		}
		else if (enumIndexType == SMD_INDEX_INDEXTYPE_BPTREE)
		{
			if (iterM->second.enumKeyType == SMD_INDEX_KEYTYPE_PAIR)
			{
				SMDIndexBp<SEARCH_KEY, PairKey_t<SEARCH_KEY> > indexSearch(m_pageMgr,sName.c_str());
				return indexSearch.searchFromFile(key,iterM->second.nOffset);
			}
			else
			{
				SMDIndexBp<SEARCH_KEY, SEARCH_KEY> indexSearch(m_pageMgr,sName.c_str());
				return indexSearch.searchFromFile(key,iterM->second.nOffset);
			}
		}
		else return SMD_INVALIDINDEX_RETURN;
	}
	else
	{
		return SMD_INVALIDINDEX_RETURN;
	}
}


//--------------------------------------------------------------------------------------------------------
// index implementation
//--------------------------------------------------------------------------------------------------------
class SMDIndexBase
{
public:
	SMDIndexBase(){};
	virtual ~SMDIndexBase(){};
public:
	virtual SMD_INDEX_KEYTYPE getIndexKeyType() = 0;
	virtual bool saveIndex(unsigned int& nRootOffset) = 0;
};

//#define BTREE_DEGREE 240
//#define BTREE_MAX_KEY (2*BTREE_DEGREE-1)
// (BTREE_DEGREE-1) <= n(关键字个数) <= 2*BTREE_DEGREE-1
// ceil( MaxKey/2 ) -1 <= n(关键字个数) <= MaxKey - 1
//template FILE_KEY 如果是复杂类型作为Key的话必须重载比较函数"<" and "==" ,以及重载iostream的"<<" and ">>"
#define BTREE_MAX_KEY 339

template<class SEARCH_KEY, class FILE_KEY>
class SMDIndex : public SMDIndexBase
{
public:
	SMDIndex(int nMaxKey) : m_sName(SMD_INDEX_NAME)
	{
		m_nMaxKey = nMaxKey;
		m_pBtreeRoot = NULL;
		m_pageMgr = NULL;
		setIndexKeyType();
	}
	SMDIndex(SMDPageManagement* pageMgr,const char* pName, unsigned int nPageSize = 4096) : m_sName(pName)
	{
		m_pBtreeRoot = NULL;

		unsigned int nRealPageSize = nPageSize-SMD_PAGEHEADER_SIZE;
		m_nMaxKey = (nRealPageSize-4)/12;	//	assume key is int(4 bytes).
		m_pageMgr = pageMgr;
		setIndexKeyType();
	}
	virtual ~SMDIndex(){clearBtreeNode();}

private:
	//B-tree struct define
    struct _BT_Key_t;
	struct _BT_Node_t;

	typedef struct _BT_Key_t
	{
		FILE_KEY key;
		unsigned int nOffset;
		typename SMDIndex::_BT_Node_t* pLeftSubTree;
		unsigned int nLSubTreeOffset; //for reading file offset of left sub-tree

		_BT_Key_t()
		{
			nOffset = SMD_INVALIDINDEX_RETURN;
			pLeftSubTree = NULL;
			nLSubTreeOffset = SMD_INVALIDINDEX_RETURN;
		}
		_BT_Key_t(const FILE_KEY& _tKey,unsigned int _nOffset=0,typename SMDIndex::_BT_Node_t* _pLSubTree=NULL,
			unsigned int nLSubOffset=0)
		{
			this->key = _tKey;
			this->nOffset = _nOffset;
			this->pLeftSubTree = _pLSubTree;
			this->nLSubTreeOffset = nLSubOffset;
		}
		~_BT_Key_t()
		{
			pLeftSubTree = NULL;
		}
		bool operator<(const struct _BT_Key_t& b) const
		{
			return this->key < b.key;
		}

		bool operator>(const struct _BT_Key_t& b) const
		{
			return this->key > b.key;
		}
	}BT_Key_t;

	typedef struct _BT_Node_t
	{	// [ceil(m / 2)-1]<= n <= m-1

		vector<BT_Key_t> vecKey;		//存储关键字（从小到大顺序） size = 2*BTREE_DEGREE-1

		unsigned int nOffset;			//记录在文件中的起始位置
		bool bLeaf;						//叶子节点标识（不含有子树的节点，非真正意义上的叶子节点）
		struct _BT_Node_t* pParent;			//指向父节点的指针（NULL代表根节点）
		struct _BT_Node_t* pRightSubTree;		//存储最右子树指针 size = 2*BTREE_DEGREE most right sub-tree pointer
		unsigned int nRSubTreeOffset;

		_BT_Node_t()
		{
			nOffset = SMD_INVALIDINDEX_RETURN;
			bLeaf = true;
			pParent = NULL;
			pRightSubTree = NULL;
			nRSubTreeOffset = SMD_INVALIDINDEX_RETURN;
		}
		~_BT_Node_t()
		{
			pParent = NULL;
			pRightSubTree = NULL;
		}

		bool IsOverflow(SMDIndex* pOuter)
		{
			return vecKey.size() >  pOuter->m_nMaxKey ? true : false;
		}
	}BT_Node_t;

	//BTree Max Keyword count
	unsigned short m_nMaxKey;
	//BTree root node
	BT_Node_t* m_pBtreeRoot;
	//for easy cleaning
	vector<BT_Node_t*> m_vecAllNodeP;
	//index name
	std::string m_sName;
	//PageManagement
	SMDPageManagement* m_pageMgr;
	//FILE_KEY type
	SMD_INDEX_KEYTYPE m_enumKeyType;
private:
	bool InsertNode(const BT_Key_t& tKey);
	void EraseCleaningNode(BT_Node_t* pInsertAt);
	bool FindInsertNode(BT_Node_t* pNode,const BT_Key_t& tKey,BT_Node_t*& pInsertNode);
	bool ConnectRightNode(BT_Node_t* pInsertAt,BT_Node_t* pRightNode);
	bool InsertAt(BT_Node_t* pInsertAt,const BT_Key_t& tKey);
	unsigned int SearchKey(BT_Node_t* pNode,const BT_Key_t& tKey);
	//for test
	void PrintAllNode(vector<BT_Node_t*>& pLevNode);
	//clean node in memory on destruct
	bool clearBtreeNode();
	//for saving btree into file
	bool saveLevNode(vector<BT_Node_t*>& vecLevNode,SMDPageFile* pFile);
	int saveOneNode(vector<BT_Node_t*>& vecNexLev,SMDPageFile* pFile,BT_Node_t* pNode);
	//for searching from file
	bool readNodeFromFile(BT_Node_t& tNode,unsigned int nOffset);
	//set index key type
	void setIndexKeyType();
public:
	// 插入<Key,Offset>到内存中,构建成一棵B-tree
	bool insertKeyOffset(const FILE_KEY& key,unsigned int& offset) { return InsertNode(BT_Key_t(key,offset)); }
	// 序列化到文件，返回在文件中的起始位置
	virtual bool saveIndex(unsigned int& nRootOffset);
	// 内存查找：根据Key来查找相应的Value（Offset），在建好树后可以在内存中查找。
	unsigned int searchFromMem(const SEARCH_KEY& key) { return SearchKey(m_pBtreeRoot,BT_Key_t(key)); }
	// 文件查找：序列化到文件后，根据Key来查找相应的Value（Offset），树序列化到文件后通过文件中的Btree查找。
	unsigned int searchFromFile(const SEARCH_KEY& key,const unsigned int& nRootOffset);
	//get index table info
	virtual SMD_INDEX_KEYTYPE getIndexKeyType() { return m_enumKeyType; };

	// 获取内容段类型
	virtual SegType getType() {	return SEG_TYPE_INDEX;}
	// 获取BTree最大关键字个数
	int getMaxKeyCnt(){return m_nMaxKey;}
	//show the tree
	void print();
};

//save to file
template<class SEARCH_KEY, class FILE_KEY>
bool SMDIndex<SEARCH_KEY, FILE_KEY>::saveIndex(unsigned int& nRootOffset)
{
	bool bRet = false;

	SMDPageFile* pPageFile = m_pageMgr->createPageFile(SEG_TYPE_INDEX,m_sName,0);
	if (!pPageFile)
		return bRet;
	pPageFile->setToFristPageFile();
	//get the start write offset
	nRootOffset = pPageFile->getPageFileBeginOffset();

	//write first time and get each node 's file offset
	vector<BT_Node_t*> vecLevNode;
	vecLevNode.push_back(m_pBtreeRoot);
	if (saveLevNode(vecLevNode,pPageFile))
	{
		//write second time: write real node's offset to file
		vecLevNode.clear();
		vecLevNode.push_back(m_pBtreeRoot);
		bRet = saveLevNode(vecLevNode,pPageFile);
	}
	
	delete pPageFile;
	pPageFile = NULL;

	return bRet;
}
template<class SEARCH_KEY, class FILE_KEY>
bool SMDIndex<SEARCH_KEY, FILE_KEY>::saveLevNode(vector<BT_Node_t*>& vecLevNode,SMDPageFile* pFile)
{
	if (vecLevNode.empty())
		return true;
	//save all node elements that belong to this level
	//and record the child tree pointer
	vector<BT_Node_t*> vecNexLev;
	typename vector<BT_Node_t*>::iterator iterV=vecLevNode.begin();
	for (; iterV!=vecLevNode.end(); ++iterV)
	{
		(*iterV)->nOffset = saveOneNode(vecNexLev,pFile,(*iterV));
		if ((*iterV)->nOffset == SMD_INVALIDINDEX_RETURN)
		{
			vecLevNode.clear();
			return false;
		}
	}
	vecLevNode.clear();
	return saveLevNode(vecNexLev,pFile);
}

//return element save offset, -1(0xFFFFFFFF) means error.  
template<class SEARCH_KEY, class FILE_KEY>
int SMDIndex<SEARCH_KEY, FILE_KEY>::saveOneNode(vector<BT_Node_t*>& vecNexLev,SMDPageFile* pFile,BT_Node_t* pNode)
{
	SMDMemStream memStream;
	//write node's size
	memStream.writeUInt(pNode->vecKey.size());

	typename vector<BT_Key_t>::iterator iterK = pNode->vecKey.begin();
	for (; iterK!=pNode->vecKey.end(); ++iterK)
	{
		memStream << iterK->key;
		memStream.writeUInt(iterK->nOffset);
		if (iterK->pLeftSubTree != NULL)
		{	//write left sub tree node address
			memStream.writeUInt(iterK->pLeftSubTree->nOffset);
			vecNexLev.push_back(iterK->pLeftSubTree);
		}
		else
		{	//not exist left sub tree node, write address 0
			memStream.writeUInt(SMD_INVALIDINDEX_RETURN);
		}
	}
	if (pNode->pRightSubTree != NULL)
	{	//write right sub tree node address
		memStream.writeUInt(pNode->pRightSubTree->nOffset);
		vecNexLev.push_back(pNode->pRightSubTree);
	}
	else
	{	//not exist right sub tree node, write address 0
		memStream.writeUInt(SMD_INVALIDINDEX_RETURN);
	}
	
	if(pNode->nOffset == SMD_INVALIDINDEX_RETURN)
	{
		return pFile->put(memStream);
	}
	else
	{
		return pFile->put(memStream,pNode->nOffset);
	}
}

template<class SEARCH_KEY, class FILE_KEY>
unsigned int SMDIndex<SEARCH_KEY, FILE_KEY>::SearchKey(BT_Node_t* pNode,const BT_Key_t& tKey)
{
	std::pair<typename vector<BT_Key_t>::iterator,typename vector<BT_Key_t>::iterator> pairPos;
	pairPos = std::equal_range(pNode->vecKey.begin(),pNode->vecKey.end(),tKey,std::less<BT_Key_t>());
	if (pairPos.second-pairPos.first == 1)
	{
		return pairPos.first->nOffset;
	}
	else if (pairPos.first == pairPos.second)
	{
		if (pairPos.first != pNode->vecKey.end())
		{
			if (pairPos.first->pLeftSubTree)
				return SearchKey(pairPos.first->pLeftSubTree,tKey);
			else
				return SMD_INVALIDINDEX_RETURN;
		}
		else
		{
			if (pNode->pRightSubTree)
				return SearchKey(pNode->pRightSubTree,tKey); 
			else 
				return SMD_INVALIDINDEX_RETURN;
		}
	}
	else
		return SMD_INVALIDINDEX_RETURN;
}

//once io in logical
template<class SEARCH_KEY, class FILE_KEY>
bool SMDIndex<SEARCH_KEY, FILE_KEY>::readNodeFromFile(BT_Node_t& tNode,unsigned int nOffset)
{
	assert(nOffset != SMD_INVALIDINDEX_RETURN);
	SMDPageFile* pPageFile = m_pageMgr->getPageFile(SEG_TYPE_INDEX,m_sName);
	if (!pPageFile)
		return false;

	//read node element size
	SMDMemStream stream;
	if(!pPageFile->get(stream,nOffset))
	{
		return false;
	}

	stream.setPosition(0);
	unsigned int nKeySize;
	stream.readUInt(nKeySize);

	for (unsigned int i=0; i<nKeySize; ++i)
	{
		BT_Key_t tKey;
		stream >> tKey.key;
		stream.readUInt(tKey.nOffset);
		stream.readUInt(tKey.nLSubTreeOffset);//this pLeftSubTree is the file offset of the left sub-tree
		tNode.vecKey.push_back(tKey);
	}
	stream.readUInt(tNode.nRSubTreeOffset);

	delete pPageFile;
	pPageFile = NULL;

	return true;
}

template<class SEARCH_KEY, class FILE_KEY>
unsigned int SMDIndex<SEARCH_KEY, FILE_KEY>::searchFromFile(const SEARCH_KEY& key,const unsigned int& nRootOffset)
{
	BT_Node_t tNode;
	if(!readNodeFromFile(tNode,nRootOffset))
	{
		return SMD_INVALIDINDEX_RETURN;
	}

	BT_Key_t tSKey(key);
	std::pair<typename vector<BT_Key_t>::iterator,typename vector<BT_Key_t>::iterator> pairPos;
	pairPos = std::equal_range(tNode.vecKey.begin(),tNode.vecKey.end(),tSKey,std::less<BT_Key_t>());
	if (pairPos.second-pairPos.first == 1)
	{
		return pairPos.first->nOffset;
	}
	else  if (pairPos.first == pairPos.second)
	{
		if (pairPos.first != tNode.vecKey.end())
		{
			if (pairPos.first->nLSubTreeOffset != SMD_INVALIDINDEX_RETURN)
				return searchFromFile(key,pairPos.first->nLSubTreeOffset);
			else
				return SMD_INVALIDINDEX_RETURN;
		}
		else
		{
			if (tNode.nRSubTreeOffset != SMD_INVALIDINDEX_RETURN)
				return searchFromFile(key,tNode.nRSubTreeOffset); 
			else 
				return SMD_INVALIDINDEX_RETURN;
		}
	}
	else return SMD_INVALIDINDEX_RETURN;
}

//Erase the cleaning node
template<class SEARCH_KEY, class FILE_KEY>
void SMDIndex<SEARCH_KEY, FILE_KEY>::EraseCleaningNode(BT_Node_t* pInsertAt)
{
	typename vector<BT_Node_t*>::iterator iterP=m_vecAllNodeP.begin();
	for (; iterP!=m_vecAllNodeP.end(); ++iterP)
	{
		if (*iterP == pInsertAt)
		{
			m_vecAllNodeP.erase(iterP);
			return;
		}
	}
}

//from m_pBTreeRoot find a leaf node for inserting a new BT_Key_t
template<class SEARCH_KEY, class FILE_KEY>
bool SMDIndex<SEARCH_KEY, FILE_KEY>::FindInsertNode(BT_Node_t* pNode,const BT_Key_t& tKey,BT_Node_t*& pInsertNode) 
{	
	//test if Key is exist
	std::pair<typename vector<BT_Key_t>::iterator,typename vector<BT_Key_t>::iterator> pairPos;
	pairPos = std::equal_range(pNode->vecKey.begin(),pNode->vecKey.end(),tKey,std::less<BT_Key_t>());
	if (pairPos.first == pairPos.second)
	{
		//find leaf node , return
		if (pNode->bLeaf)
		{
			pInsertNode = pNode;
			return true;
		}	
		if (pairPos.first != pNode->vecKey.end())
		{
			return FindInsertNode(pairPos.first->pLeftSubTree,tKey,pInsertNode);
		}
		else // the node's most right sub-tree
		{
			return FindInsertNode(pNode->pRightSubTree,tKey,pInsertNode);
		}
	}
	else //can't insert same key 
	{
		return false;
	}
}

//find the insert position's next Key, the Key's pLeftSubTree should be the pRightNode
template<class SEARCH_KEY, class FILE_KEY>
bool SMDIndex<SEARCH_KEY, FILE_KEY>::ConnectRightNode(BT_Node_t* pInsertAt,BT_Node_t* pRightNode)
{	//get parent pointer
	BT_Node_t* pParent = pInsertAt->pParent;
	//if pInsertAt is the parent's most right sub-tree pointer or
	//if pParent is the newly created root
	if (pParent->pRightSubTree == pInsertAt || pParent->pRightSubTree == NULL)
	{
		pParent->pRightSubTree = pRightNode;
		return true;
	}
	//
	typename vector<BT_Key_t>::iterator iterK = pParent->vecKey.begin();
	for (; iterK!=pParent->vecKey.end(); ++iterK)
	{
		if (iterK->pLeftSubTree == pInsertAt)
		{
			iterK->pLeftSubTree = pRightNode;
			return true;
		}
	}
	
	return false;
}

template<class SEARCH_KEY, class FILE_KEY>
bool SMDIndex<SEARCH_KEY, FILE_KEY>::InsertAt(BT_Node_t* pInsertAt,const BT_Key_t& tKey)
{
	pInsertAt->vecKey.push_back(tKey);
	std::sort(pInsertAt->vecKey.begin(),pInsertAt->vecKey.end(),std::less<BT_Key_t>());

	//overflow...split tree node
	if (pInsertAt->IsOverflow(this)) 
	{	//pInsertAt is the root and overflowed,so create new root as parent
		if (!pInsertAt->pParent)
		{
			BT_Node_t* pBtRootNode = new BT_Node_t();
			pBtRootNode->bLeaf = false;
			pBtRootNode->pParent = NULL;
			pBtRootNode->pRightSubTree = NULL;

			m_pBtreeRoot = pBtRootNode;
			pInsertAt->pParent = m_pBtreeRoot;

			m_vecAllNodeP.push_back(pBtRootNode);
		}
		//find middle
		int nMid = pInsertAt->vecKey.size()/2;
		//new left nodes sub-tree
		BT_Node_t* pLeftNode = new BT_Node_t;
		pLeftNode->bLeaf = pInsertAt->bLeaf;
		pLeftNode->pParent = pInsertAt->pParent;
		//copy the record where laying on the left of the middle record to the pLeftNode
		//if the pLeftNode is not leaf node, change the child node's parent to the new created pLeftNode
		for (int i=0; i<nMid; ++i)
		{
			pLeftNode->vecKey.push_back(pInsertAt->vecKey[i]);
			if (!pLeftNode->bLeaf)
				pInsertAt->vecKey[i].pLeftSubTree->pParent = pLeftNode;
		}
		//new right nodes sub-tree
		BT_Node_t* pRightNode = new BT_Node_t;
		pRightNode->bLeaf = pInsertAt->bLeaf;
		pRightNode->pParent = pInsertAt->pParent;
		//copy the record where laying on the right of the middle record to the pRightNode
		//if the pRightNode is not leaf node, change the child node's parent to the new created pRightNode
		for (unsigned int j=nMid+1; j<pInsertAt->vecKey.size(); ++j)
		{
			pRightNode->vecKey.push_back(pInsertAt->vecKey[j]);
			if (!pRightNode->bLeaf)
				pInsertAt->vecKey[j].pLeftSubTree->pParent = pRightNode;
		}

		if (!pInsertAt->bLeaf)
		{	//put the middle record's pLeftSubTree pointer to pLeftNode's pRightSubTree
			pLeftNode->pRightSubTree = pInsertAt->vecKey[nMid].pLeftSubTree;
			pInsertAt->vecKey[nMid].pLeftSubTree->pParent = pLeftNode;
			//put the pInsertAt's pRightSubTree pointer to pRightNode's pRightSubTree
			pRightNode->pRightSubTree = pInsertAt->pRightSubTree;
			pInsertAt->pRightSubTree->pParent = pRightNode;
		}

		//put the pLeftNode pointer to the middle record's pLeftSubTree
		pInsertAt->vecKey[nMid].pLeftSubTree = pLeftNode;
		//put the pRightNode pointer to the ParentKey's pLeftSubTree,
		//if the ParentKey is NULL,then put the pRightNode pointer to the pInsertAt->pParent->pRightSubTree
		//if the pInsertAt->pParent is NULL(it means pInsertAt is Root), then create the new root. 
		if (!ConnectRightNode(pInsertAt,pRightNode))
		{
			delete pLeftNode;
			delete pRightNode;
			pLeftNode = NULL;
			pRightNode = NULL;
			return false;
		}

		//insert the middle record to the pInsertAt->pParent
		InsertAt(pInsertAt->pParent,pInsertAt->vecKey[nMid]);

		//delete the present node
		EraseCleaningNode(pInsertAt);
		delete pInsertAt;
		pInsertAt = NULL;

		//for cleaning
		m_vecAllNodeP.push_back(pLeftNode);
		m_vecAllNodeP.push_back(pRightNode);
	}

	return true;
}

//inner class function
template<class SEARCH_KEY, class FILE_KEY>
bool SMDIndex<SEARCH_KEY, FILE_KEY>::InsertNode(const BT_Key_t& tKey)
{
	//Btree is null
	if (!m_pBtreeRoot)
	{	//create new node
		BT_Node_t* pBtNode = new BT_Node_t();
		pBtNode->vecKey.push_back(tKey);
		//pBtNode->nKeyCnt++;
		pBtNode->bLeaf = true;
		pBtNode->pParent = NULL;
		pBtNode->pRightSubTree = NULL;

		m_pBtreeRoot = pBtNode;

		m_vecAllNodeP.push_back(pBtNode);
		return true;
	}

	//find the leaf node to insert
	BT_Node_t* pInsertNode = NULL;
	if (!FindInsertNode(m_pBtreeRoot,tKey,pInsertNode))
		return false;

	//insert node
	return InsertAt(pInsertNode,tKey);
}
template<class SEARCH_KEY, class FILE_KEY>
bool SMDIndex<SEARCH_KEY, FILE_KEY>::clearBtreeNode()
{
	typename vector<BT_Node_t*>::iterator iterP=m_vecAllNodeP.begin();
	for (; iterP!=m_vecAllNodeP.end(); ++iterP)
	{
		delete *iterP;
	}
	return true;
}
template<class SEARCH_KEY, class FILE_KEY>
void SMDIndex<SEARCH_KEY, FILE_KEY>::setIndexKeyType()
{
	std::string strTName(typeid(FILE_KEY).name());
	if (strTName.find("int") != std::string::npos)
	{
		m_enumKeyType = SMD_INDEX_KEYTYPE_INT;
	}
	else if (strTName.find("unsigned int") != std::string::npos)
	{
		m_enumKeyType = SMD_INDEX_KEYTYPE_UINT;
	}
	else if (strTName.find("double") != std::string::npos)
	{
		m_enumKeyType = SMD_INDEX_KEYTYPE_DOUBLE;
	}
	else if (strTName.find("std::basic_string") != std::string::npos)
	{
		m_enumKeyType = SMD_INDEX_KEYTYPE_STRING;
	}
	else if (strTName.find("PairKey_t") != std::string::npos)
	{
		m_enumKeyType = SMD_INDEX_KEYTYPE_PAIR;
	}
	else
	{
		m_enumKeyType = SMD_INDEX_KEYTYPE_NULL;
	}
}

template<class SEARCH_KEY, class FILE_KEY>
void SMDIndex<SEARCH_KEY, FILE_KEY>::print()
{
	vector<BT_Node_t*> vecRoot;
	vecRoot.push_back(m_pBtreeRoot);
	PrintAllNode(vecRoot);
}

template<class SEARCH_KEY, class FILE_KEY>
void SMDIndex<SEARCH_KEY, FILE_KEY>::PrintAllNode(vector<BT_Node_t*>& vecLevNode)
{
	if (vecLevNode.empty())
		return;
	//print all node elements that belong to this level
	//and record the child tree pointer
	vector<BT_Node_t*> vecNexLev;

	typename vector<BT_Node_t*>::iterator iterV=vecLevNode.begin();
	for (; iterV!=vecLevNode.end(); ++iterV)
	{
		//m_pBtreeRoot->vecKey
		cout << "(";
		typename vector<BT_Key_t>::iterator iterK=(*iterV)->vecKey.begin();
		for (; iterK!=(*iterV)->vecKey.end(); )
		{
			cout << iterK->key;
			typename vector<BT_Key_t>::iterator iterT = iterK + 1;
			if (iterT != (*iterV)->vecKey.end())
			{
				cout << ",";
			}
			if (iterK->pLeftSubTree != NULL)
				vecNexLev.push_back(iterK->pLeftSubTree);
			++iterK;
		}
		cout << ") ";
		if ((*iterV)->pRightSubTree != NULL)
			vecNexLev.push_back((*iterV)->pRightSubTree);
	}
	cout << endl;
	vecLevNode.clear();

	PrintAllNode(vecNexLev);
}

//-----------------------------------------------------------------------------------------------------
// B+ tree implementation
//-----------------------------------------------------------------------------------------------------

// (BTREE_DEGREE-1) <= n(关键字个数) <= 2*BTREE_DEGREE-1
// ceil( MaxKey/2 ) -1 <= n(关键字个数) <= MaxKey - 1
//template FILE_KEY 如果是复杂类型作为Key的话必须重载比较函数"<" and "==" ,以及重载iostream的"<<" and ">>"
#define BPTREE_MAX_KEY 509

template<class SEARCH_KEY, class FILE_KEY>
class SMDIndexBp : public SMDIndexBase
{
public:
	SMDIndexBp(int nMaxKey) : m_sName(SMD_INDEX_NAME)
	{
		m_nMaxKey = nMaxKey;
		m_pBtreeRoot = NULL;
		m_pageMgr = NULL;
		setIndexKeyType();
	}
	SMDIndexBp(SMDPageManagement* pageMgr,const char* pName, unsigned int nPageSize = 4096) : m_sName(pName)
	{
		m_pBtreeRoot = NULL;

		unsigned int nRealPageSize = nPageSize-SMD_PAGEHEADER_SIZE;
		m_nMaxKey = (nRealPageSize-5)/8;	//	assume key is int(4 bytes).
		m_pageMgr = pageMgr;
		setIndexKeyType();
	}
	virtual ~SMDIndexBp(){clearBPtreeNode();}

private:
	//B-tree struct define
	struct _BT_Key_t;
	struct _BT_Node_t;

	typedef struct _BT_Key_t
	{
		FILE_KEY key;
		typename SMDIndexBp::_BT_Node_t* pLeftSubTree;
		unsigned int nLSubTreeOffset; //for reading file offset of left sub-tree
		                              //if it is leaf, use for saving real offset

		_BT_Key_t()
		{
			pLeftSubTree = NULL;
			nLSubTreeOffset = SMD_INVALIDINDEX_RETURN;
		}
		_BT_Key_t(const FILE_KEY& _tKey,unsigned int nLSubOffset=SMD_INVALIDINDEX_RETURN,
			typename SMDIndexBp::_BT_Node_t* _pLSubTree=NULL)
		{
			this->key = _tKey;
			this->nLSubTreeOffset = nLSubOffset;
			this->pLeftSubTree = _pLSubTree;
		}
		~_BT_Key_t()
		{
			pLeftSubTree = NULL;
		}
		bool operator<(const struct _BT_Key_t& b) const
		{
			return this->key < b.key;
		}

		bool operator>(const struct _BT_Key_t& b) const
		{
			return this->key > b.key;
		}
	}BT_Key_t;

	typedef struct _BT_Node_t
	{	// [ceil(m / 2)-1]<= n <= m-1

		vector<BT_Key_t> vecKey;		//存储关键字（从小到大顺序） size = 2*BTREE_DEGREE-1

		unsigned int nOffset;				//记录在文件中的起始位置(used for writing node to file)
		bool bLeaf;							//叶子节点标识（不含有子树的节点）
		struct _BT_Node_t* pParent;			//指向父节点的指针（NULL代表根节点）
		struct _BT_Node_t* pRightSubTree;	//存储最右子树指针 size = 2*BTREE_DEGREE most right sub-tree pointer
		unsigned int nRSubTreeOffset;

		_BT_Node_t()
		{
			nOffset = SMD_INVALIDINDEX_RETURN;
			bLeaf = true;
			pParent = NULL;
			pRightSubTree = NULL;
			nRSubTreeOffset = SMD_INVALIDINDEX_RETURN;
		}
		~_BT_Node_t()
		{
			pParent = NULL;
			pRightSubTree = NULL;
		}

		bool IsOverflow(SMDIndexBp* pOuter)
		{
			return vecKey.size() >  pOuter->m_nMaxKey ? true : false;
		}
	}BT_Node_t;

	//BPTree Max Keyword count
	unsigned short m_nMaxKey;
	//BPTree root node
	BT_Node_t* m_pBtreeRoot;
	//for easy cleaning
	vector<BT_Node_t*> m_vecAllNodeP;
	//index name
	std::string m_sName;
	//PageManagement
	SMDPageManagement* m_pageMgr;
	//FILE_KEY type
	SMD_INDEX_KEYTYPE m_enumKeyType;
private:
	bool InsertNode(const BT_Key_t& tKey);
	void EraseCleaningNode(BT_Node_t* pInsertAt);
	bool FindInsertNode(BT_Node_t* pNode,const BT_Key_t& tKey,BT_Node_t*& pInsertNode);
	bool ConnectRightNode(BT_Node_t* pInsertAt,BT_Node_t* pRightNode);
	bool InsertAt(BT_Node_t* pInsertAt,const BT_Key_t& tKey);
	unsigned int SearchKey(BT_Node_t* pNode,const BT_Key_t& tKey);
	//for test
	void PrintAllNode(vector<BT_Node_t*>& pLevNode);
	//clean node in memory on destruct
	bool clearBPtreeNode();
	//for saving bptree into file
	bool saveLevNode(vector<BT_Node_t*>& vecLevNode,SMDPageFile* pFile);
	int saveOneNode(vector<BT_Node_t*>& vecNexLev,SMDPageFile* pFile,BT_Node_t* pNode);
	//for searching from file
	bool readNodeFromFile(BT_Node_t& tNode,unsigned int nOffset);
	//set index key type
	void setIndexKeyType();
public:
	// 插入<Key,Offset>到内存中,构建成一棵B+_tree
	bool insertKeyOffset(const FILE_KEY& key,unsigned int& offset) { return InsertNode(BT_Key_t(key,offset)); }
	// 序列化到文件，返回在文件中的起始位置
	virtual bool saveIndex(unsigned int& nRootOffset);
	// 内存查找：根据Key来查找相应的Value（Offset），在建好树后可以在内存中查找。
	unsigned int searchFromMem(const SEARCH_KEY& key) { return SearchKey(m_pBtreeRoot,BT_Key_t(key)); }
	// 文件查找：序列化到文件后，根据Key来查找相应的Value（Offset），树序列化到文件后通过文件中的BPtree查找。
	unsigned int searchFromFile(const SEARCH_KEY& key,const unsigned int& nRootOffset);
	//get index table info
	virtual SMD_INDEX_KEYTYPE getIndexKeyType() { return m_enumKeyType; };

	// 获取内容段类型
	virtual SegType getType() {	return SEG_TYPE_INDEX;}
	// 获取BPTree最大关键字个数
	int getMaxKeyCnt(){return m_nMaxKey;}
	//show the tree
	void print();
};

//save to file
template<class SEARCH_KEY, class FILE_KEY>
bool SMDIndexBp<SEARCH_KEY, FILE_KEY>::saveIndex(unsigned int& nRootOffset)
{
	bool bRet = false;

	SMDPageFile* pPageFile = m_pageMgr->createPageFile(SEG_TYPE_INDEX,m_sName,0);
	if (!pPageFile)
		return bRet;
	pPageFile->setToFristPageFile();
	//get the start write offset
	nRootOffset = pPageFile->getPageFileBeginOffset();

	//write first time and get each node 's file offset
	vector<BT_Node_t*> vecLevNode;
	vecLevNode.push_back(m_pBtreeRoot);
	if (saveLevNode(vecLevNode,pPageFile))
	{
		//write second time: write real node's offset to file
		vecLevNode.clear();
		vecLevNode.push_back(m_pBtreeRoot);
		bRet = saveLevNode(vecLevNode,pPageFile);
	}

	delete pPageFile;
	pPageFile = NULL;

	return bRet;
}
template<class SEARCH_KEY, class FILE_KEY>
bool SMDIndexBp<SEARCH_KEY, FILE_KEY>::saveLevNode(vector<BT_Node_t*>& vecLevNode,SMDPageFile* pFile)
{
	if (vecLevNode.empty())
		return true;
	//save all node elements that belong to this level
	//and record the child tree pointer
	vector<BT_Node_t*> vecNexLev;
	typename vector<BT_Node_t*>::iterator iterV=vecLevNode.begin();
	for (; iterV!=vecLevNode.end(); ++iterV)
	{
		(*iterV)->nOffset = saveOneNode(vecNexLev,pFile,(*iterV));
		if ((*iterV)->nOffset == SMD_INVALIDINDEX_RETURN)
		{
			vecLevNode.clear();
			return false;
		}
	}
	vecLevNode.clear();
	return saveLevNode(vecNexLev,pFile);
}

//return element save offset, -1(0xFFFFFFFF) means error.  
template<class SEARCH_KEY, class FILE_KEY>
int SMDIndexBp<SEARCH_KEY, FILE_KEY>::saveOneNode(vector<BT_Node_t*>& vecNexLev,SMDPageFile* pFile,BT_Node_t* pNode)
{
	SMDMemStream memStream;
	//write node's size
	memStream.writeUInt(pNode->vecKey.size());

	typename vector<BT_Key_t>::iterator iterK = pNode->vecKey.begin();
	for (; iterK!=pNode->vecKey.end(); ++iterK)
	{
		memStream << iterK->key;
		if (iterK->pLeftSubTree != NULL)
		{	//write left sub tree node address
			memStream.writeUInt(iterK->pLeftSubTree->nOffset);
			vecNexLev.push_back(iterK->pLeftSubTree);
		}
		else
		{	//not exist left sub tree node, means the leaf node.
			memStream.writeUInt(iterK->nLSubTreeOffset);  //save the real data offset
		}
	}
	if (pNode->pRightSubTree != NULL)
	{	//write right sub tree node address
		memStream.writeUInt(pNode->pRightSubTree->nOffset);
		vecNexLev.push_back(pNode->pRightSubTree);
	}
	else
	{	//not exist right sub tree node, write address 0
		memStream.writeUInt(SMD_INVALIDINDEX_RETURN);
	}
	//save leaf flag
	if (pNode->bLeaf)
	{
		unsigned char bLeaf = 0;
		bLeaf &= 0x0;
		bLeaf |= 0x01;
		memStream.writeBytes((void*)&bLeaf,1);
	}
	else
	{
		unsigned char bLeaf = 0;
		bLeaf &= 0x0;
		memStream.writeBytes((void*)&bLeaf,1);
	}

	if(pNode->nOffset == SMD_INVALIDINDEX_RETURN)
	{
		return pFile->put(memStream);
	}
	else
	{
		return pFile->put(memStream,pNode->nOffset);
	}
}

template<class SEARCH_KEY, class FILE_KEY>
unsigned int SMDIndexBp<SEARCH_KEY, FILE_KEY>::SearchKey(BT_Node_t* pNode,const BT_Key_t& tKey)
{
	std::pair<typename vector<BT_Key_t>::iterator,typename vector<BT_Key_t>::iterator> pairPos;
	pairPos = std::equal_range(pNode->vecKey.begin(),pNode->vecKey.end(),tKey,std::less<BT_Key_t>());
	if (pairPos.second-pairPos.first == 1)
	{
		BT_Node_t node_end =pNode->vecKey.end();
		if (pNode->bLeaf)
		{
			return pairPos.first->nLSubTreeOffset;//on leaf node ,the "nLSubTreeOffset" is the real data Offset.
		}
		else if (pairPos.second == node_end)
		{
			return SearchKey(pNode->pRightSubTree,tKey);
		}
		else
		{
			return SearchKey(pairPos.second->pLeftSubTree,tKey);
		}
	}
	else if (pairPos.first == pairPos.second)
	{
		if (pNode->bLeaf)
		{
			return SMD_INVALIDINDEX_RETURN;
		}
		else
		{
			if (pairPos.first != pNode->vecKey.end())
			{
				if (pairPos.first->pLeftSubTree)
					return SearchKey(pairPos.first->pLeftSubTree,tKey);
				else
					return SMD_INVALIDINDEX_RETURN;
			}
			else
			{
				if (pNode->pRightSubTree)
					return SearchKey(pNode->pRightSubTree,tKey); 
				else 
					return SMD_INVALIDINDEX_RETURN;
			}
		}
	}
	else
		return SMD_INVALIDINDEX_RETURN;
}

//once io in logical
template<class SEARCH_KEY, class FILE_KEY>
bool SMDIndexBp<SEARCH_KEY, FILE_KEY>::readNodeFromFile(BT_Node_t& tNode,unsigned int nOffset)
{
	//assert(nOffset != SMD_INVALIDINDEX_RETURN);
	if (nOffset == SMD_INVALIDINDEX_RETURN)
	{
		return false;
	}
	SMDPageFile* pPageFile = m_pageMgr->getPageFile(SEG_TYPE_INDEX,m_sName);
	if (!pPageFile)
		return false;

	//read node element size
	SMDMemStream stream;
	if(!pPageFile->get(stream,nOffset))
	{
		return false;
	}

	stream.setPosition(0);
	unsigned int nKeySize;
	stream.readUInt(nKeySize);

	for (unsigned int i=0; i<nKeySize; ++i)
	{
		BT_Key_t tKey;
		stream >> tKey.key;
		stream.readUInt(tKey.nLSubTreeOffset);
		tNode.vecKey.push_back(tKey);
	}
	stream.readUInt(tNode.nRSubTreeOffset);

	unsigned char bLeaf = 0;
	stream.readBytes((void*)&bLeaf,1);
	if ((bLeaf & 0x01) == 1)
	{
		tNode.bLeaf = true;
	}
	else
	{
		tNode.bLeaf = false;
	}

	delete pPageFile;
	pPageFile = NULL;

	return true;
}

template<class SEARCH_KEY, class FILE_KEY>
unsigned int SMDIndexBp<SEARCH_KEY, FILE_KEY>::searchFromFile(const SEARCH_KEY& key,const unsigned int& nRootOffset)
{
	BT_Node_t tNode;
	if(!readNodeFromFile(tNode,nRootOffset))
	{
		return SMD_INVALIDINDEX_RETURN;
	}

	BT_Key_t tSKey(key);
	std::pair<typename vector<BT_Key_t>::iterator,typename vector<BT_Key_t>::iterator> pairPos;
	pairPos = std::equal_range(tNode.vecKey.begin(),tNode.vecKey.end(),tSKey,std::less<BT_Key_t>());
	if (pairPos.second-pairPos.first == 1)
	{
		if (tNode.bLeaf)
		{
			return pairPos.first->nLSubTreeOffset; //on leaf node ,the "nLSubTreeOffset" is the real data Offset.
		}
		else if (pairPos.second == tNode.vecKey.end())
		{
			return searchFromFile(key,tNode.nRSubTreeOffset);
		}
		else
		{
			return searchFromFile(key,pairPos.second->nLSubTreeOffset);
		}
	}
	else  if (pairPos.first == pairPos.second)
	{
		if (tNode.bLeaf)
		{
			return SMD_INVALIDINDEX_RETURN;
		}
		else
		{
			if (pairPos.first != tNode.vecKey.end())
			{
				return searchFromFile(key,pairPos.first->nLSubTreeOffset);
			}
			else
			{
				return searchFromFile(key,tNode.nRSubTreeOffset);
			}
		}
	}
	else return SMD_INVALIDINDEX_RETURN;
}

//Erase the cleaning node
template<class SEARCH_KEY, class FILE_KEY>
void SMDIndexBp<SEARCH_KEY, FILE_KEY>::EraseCleaningNode(BT_Node_t* pInsertAt)
{
	typename vector<BT_Node_t*>::iterator iterP=m_vecAllNodeP.begin();
	for (; iterP!=m_vecAllNodeP.end(); ++iterP)
	{
		if (*iterP == pInsertAt)
		{
			m_vecAllNodeP.erase(iterP);
			return;
		}
	}
}

//from m_pBTreeRoot find a leaf node for inserting a new BT_Key_t
template<class SEARCH_KEY, class FILE_KEY>
bool SMDIndexBp<SEARCH_KEY, FILE_KEY>::FindInsertNode(BT_Node_t* pNode,const BT_Key_t& tKey,BT_Node_t*& pInsertNode) 
{	
	//test if Key is exist
	std::pair<typename vector<BT_Key_t>::iterator,typename vector<BT_Key_t>::iterator> pairPos;
	pairPos = std::equal_range(pNode->vecKey.begin(),pNode->vecKey.end(),tKey,std::less<BT_Key_t>());
	if (pairPos.first == pairPos.second)
	{
		//find leaf node , return
		if (pNode->bLeaf)
		{
			pInsertNode = pNode;
			return true;
		}
		if (pairPos.first != pNode->vecKey.end())
		{
			return FindInsertNode(pairPos.first->pLeftSubTree,tKey,pInsertNode);
		}
		else // the node's most right sub-tree
		{
			return FindInsertNode(pNode->pRightSubTree,tKey,pInsertNode);
		}
	}
	else //can't insert same key 
	{
		return false;
	}
}

//find the insert position's next Key, the Key's pLeftSubTree should be the pRightNode
template<class SEARCH_KEY, class FILE_KEY>
bool SMDIndexBp<SEARCH_KEY, FILE_KEY>::ConnectRightNode(BT_Node_t* pInsertAt,BT_Node_t* pRightNode)
{	//get parent pointer
	BT_Node_t* pParent = pInsertAt->pParent;
	//if pInsertAt is the parent's most right sub-tree pointer or
	//if pParent is the newly created root
	if (pParent->pRightSubTree == pInsertAt || pParent->pRightSubTree == NULL)
	{
		pParent->pRightSubTree = pRightNode;
		return true;
	}
	//
	typename vector<BT_Key_t>::iterator iterK = pParent->vecKey.begin();
	for (; iterK!=pParent->vecKey.end(); ++iterK)
	{
		if (iterK->pLeftSubTree == pInsertAt)
		{
			iterK->pLeftSubTree = pRightNode;
			return true;
		}
	}

	return false;
}

template<class SEARCH_KEY, class FILE_KEY>
bool SMDIndexBp<SEARCH_KEY, FILE_KEY>::InsertAt(BT_Node_t* pInsertAt,const BT_Key_t& tKey)
{
	pInsertAt->vecKey.push_back(tKey);
	std::sort(pInsertAt->vecKey.begin(),pInsertAt->vecKey.end(),std::less<BT_Key_t>());

	//overflow...split tree node
	if (pInsertAt->IsOverflow(this)) 
	{	//pInsertAt is the root and overflowed,so create new root as parent
		if (!pInsertAt->pParent)
		{
			BT_Node_t* pBtRootNode = new BT_Node_t();
			pBtRootNode->bLeaf = false;
			pBtRootNode->pParent = NULL;
			pBtRootNode->pRightSubTree = NULL;

			m_pBtreeRoot = pBtRootNode;
			pInsertAt->pParent = m_pBtreeRoot;

			m_vecAllNodeP.push_back(pBtRootNode);
		}
		//find middle
		int nMid = pInsertAt->vecKey.size()/2;
		//new left nodes sub-tree
		BT_Node_t* pLeftNode = new BT_Node_t;
		pLeftNode->bLeaf = pInsertAt->bLeaf;
		pLeftNode->pParent = pInsertAt->pParent;
		//copy the record where laying on the left of the middle record to the pLeftNode
		//if the pLeftNode is not leaf node, change the child node's parent to the new created pLeftNode
		for (int i=0; i<nMid; ++i)
		{
			pLeftNode->vecKey.push_back(pInsertAt->vecKey[i]);
			if (!pLeftNode->bLeaf)
				pInsertAt->vecKey[i].pLeftSubTree->pParent = pLeftNode;
		}
		//new right nodes sub-tree
		BT_Node_t* pRightNode = new BT_Node_t;
		pRightNode->bLeaf = pInsertAt->bLeaf;
		pRightNode->pParent = pInsertAt->pParent;
		//copy the record where laying on the right of the middle record to the pRightNode
		//if the pRightNode is not leaf node, change the child node's parent to the new created pRightNode
		unsigned int nCopyFrom = 0;
		if (pInsertAt->bLeaf)
			nCopyFrom = nMid;
		else nCopyFrom = nMid+1;
		for (unsigned int j=nCopyFrom; j<pInsertAt->vecKey.size(); ++j)
		{
			pRightNode->vecKey.push_back(pInsertAt->vecKey[j]);
			if (!pRightNode->bLeaf)
				pInsertAt->vecKey[j].pLeftSubTree->pParent = pRightNode;
		}

		if (!pInsertAt->bLeaf)
		{	//put the middle record's pLeftSubTree pointer to pLeftNode's pRightSubTree
			pLeftNode->pRightSubTree = pInsertAt->vecKey[nMid].pLeftSubTree;
			pInsertAt->vecKey[nMid].pLeftSubTree->pParent = pLeftNode;
			//put the pInsertAt's pRightSubTree pointer to pRightNode's pRightSubTree
			pRightNode->pRightSubTree = pInsertAt->pRightSubTree;
			pInsertAt->pRightSubTree->pParent = pRightNode;
		}

		//put the pLeftNode pointer to the middle record's pLeftSubTree
		pInsertAt->vecKey[nMid].pLeftSubTree = pLeftNode;
		//put the pRightNode pointer to the ParentKey's pLeftSubTree,
		//if the ParentKey is NULL,then put the pRightNode pointer to the pInsertAt->pParent->pRightSubTree
		//if the pInsertAt->pParent is NULL(it means pInsertAt is Root), then create the new root. 
		if (!ConnectRightNode(pInsertAt,pRightNode))
		{
			delete pLeftNode;
			delete pRightNode;
			pLeftNode = NULL;
			pRightNode = NULL;
			return false;
		}

		//insert the middle record to the pInsertAt->pParent
		InsertAt(pInsertAt->pParent,pInsertAt->vecKey[nMid]);

		//delete the present node
		EraseCleaningNode(pInsertAt);
		delete pInsertAt;
		pInsertAt = NULL;

		//for cleaning
		m_vecAllNodeP.push_back(pLeftNode);
		m_vecAllNodeP.push_back(pRightNode);
	}

	return true;
}

//inner class function
template<class SEARCH_KEY, class FILE_KEY>
bool SMDIndexBp<SEARCH_KEY, FILE_KEY>::InsertNode(const BT_Key_t& tKey)
{
	//Btree is null
	if (!m_pBtreeRoot)
	{	//create new node
		BT_Node_t* pBtNode = new BT_Node_t();
		pBtNode->vecKey.push_back(tKey);
		//pBtNode->nKeyCnt++;
		pBtNode->bLeaf = true;
		pBtNode->pParent = NULL;
		pBtNode->pRightSubTree = NULL;

		m_pBtreeRoot = pBtNode;

		m_vecAllNodeP.push_back(pBtNode);
		return true;
	}

	//find the leaf node to insert
	BT_Node_t* pInsertNode = NULL;
	if (!FindInsertNode(m_pBtreeRoot,tKey,pInsertNode))
		return false;

	//insert node
	return InsertAt(pInsertNode,tKey);
}
template<class SEARCH_KEY, class FILE_KEY>
bool SMDIndexBp<SEARCH_KEY, FILE_KEY>::clearBPtreeNode()
{
	typename vector<BT_Node_t*>::iterator iterP=m_vecAllNodeP.begin();
	for (; iterP!=m_vecAllNodeP.end(); ++iterP)
	{
		delete *iterP;
	}
	return true;
}
template<class SEARCH_KEY, class FILE_KEY>
void SMDIndexBp<SEARCH_KEY, FILE_KEY>::setIndexKeyType()
{
	std::string strTName(typeid(FILE_KEY).name());
	if (strTName.find("int") != std::string::npos)
	{
		m_enumKeyType = SMD_INDEX_KEYTYPE_INT;
	}
	else if (strTName.find("unsigned int") != std::string::npos)
	{
		m_enumKeyType = SMD_INDEX_KEYTYPE_UINT;
	}
	else if (strTName.find("double") != std::string::npos)
	{
		m_enumKeyType = SMD_INDEX_KEYTYPE_DOUBLE;
	}
	else if (strTName.find("std::basic_string") != std::string::npos)
	{
		m_enumKeyType = SMD_INDEX_KEYTYPE_STRING;
	}
	else if (strTName.find("PairKey_t") != std::string::npos)
	{
		m_enumKeyType = SMD_INDEX_KEYTYPE_PAIR;
	}
	else
	{
		m_enumKeyType = SMD_INDEX_KEYTYPE_NULL;
	}
}

template<class SEARCH_KEY, class FILE_KEY>
void SMDIndexBp<SEARCH_KEY, FILE_KEY>::print()
{
	vector<BT_Node_t*> vecRoot;
	vecRoot.push_back(m_pBtreeRoot);
	PrintAllNode(vecRoot);
}

template<class SEARCH_KEY, class FILE_KEY>
void SMDIndexBp<SEARCH_KEY, FILE_KEY>::PrintAllNode(vector<BT_Node_t*>& vecLevNode)
{
	if (vecLevNode.empty())
		return;
	//print all node elements that belong to this level
	//and record the child tree pointer
	vector<BT_Node_t*> vecNexLev;

	typename vector<BT_Node_t*>::iterator iterV=vecLevNode.begin();
	for (; iterV!=vecLevNode.end(); ++iterV)
	{
		//m_pBtreeRoot->vecKey
		cout << "(";
		typename vector<BT_Key_t>::iterator iterK=(*iterV)->vecKey.begin();
		for (; iterK!=(*iterV)->vecKey.end(); )
		{
			cout << iterK->key;
			typename vector<BT_Key_t>::iterator iterT = iterK + 1;
			if (iterT != (*iterV)->vecKey.end())
			{
				cout << ",";
			}
			if (iterK->pLeftSubTree != NULL)
				vecNexLev.push_back(iterK->pLeftSubTree);
			++iterK;
		}
		cout << ") ";
		if ((*iterV)->pRightSubTree != NULL)
			vecNexLev.push_back((*iterV)->pRightSubTree);
	}
	cout << endl;
	vecLevNode.clear();

	PrintAllNode(vecNexLev);
}

}//namespace

#endif
