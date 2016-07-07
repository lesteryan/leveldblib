
#ifndef __DEV_BUFFER_H__
#define __DEV_BUFFER_H__

#include <string>

using std::string;

typedef unsigned short TPageNum;

class CPageMap
{
public:
    static const unsigned short BUFFER_NULL = 0xffff;

public:
    CPageMap() : _nMaxPageNum(0)
	{
	}

	~CPageMap()
	{
        delete _pnArrayPages;
		_pnArrayPages = NULL;
	}

	bool init(unsigned short nMaxPageNum)
	{
		_nMaxPageNum = nMaxPageNum;
        _pnArrayPages = new unsigned short[_nMaxPageNum];
		for (int nPageNo=0; nPageNo<_nMaxPageNum; ++nPageNo)
		{
            _pnArrayPages[nPageNo] = BUFFER_NULL;
		}

		_bInitFinish = true;
		return true;
	}

	unsigned short getBufferNo(int nPageNo)
	{
        if (!_bInitFinish){
            return BUFFER_NULL;
        }

        return _pnArrayPages[nPageNo];
    }

	inline bool addPageMap(unsigned short nPageNo, unsigned short nBufferNo)
	{
        if (!_bInitFinish || nPageNo >= _nMaxPageNum){
            return false;
        }

        _pnArrayPages[nPageNo] = nBufferNo;
        return true;
    }

	inline bool delPageMap(unsigned short nDelPageNo)
	{
        if (!_bInitFinish || nDelPageNo >= _nMaxPageNum){
            return false;
        }

        _pnArrayPages[nDelPageNo] = BUFFER_NULL;
        return true;
    }

private:
    bool _bInitFinish;

    unsigned short _nMaxPageNum;
    unsigned short *_pnArrayPages;
};

class CPriorityQue
{
    typedef struct CQueNode{
        CQueNode(unsigned short nBufNo){
            ptNext = ptPrv = NULL;
            _nBufNo = nBufNo;
        }

        struct CQueNode *ptNext;
        struct CQueNode *ptPrv;
        unsigned short _nBufNo;
    }CQueNode;

    typedef struct CBufNode{
        CQueNode *ptNode;
        unsigned short nPageNo;
    }CBufNode;

public:
    static const unsigned short NO_PAGE = 0xffff;
public:
    CPriorityQue() : _nElemNum(0), _poHead(NULL), _nSwapPageTimes(0), _nHitTimes(0)
    {
    }

    ~CPriorityQue()
    {
		if (NULL != _poHead) {
		    CQueNode *poNode = _poHead->ptNext, *poPrv = NULL;
			while (poNode != _poHead) {
			    poPrv = poNode;
				poNode = poNode->ptNext;
				delete poPrv;
			}
			delete _poHead;
			_poHead = NULL;
		}

		if (NULL != _poArrayBuf) {
		    delete _poArrayBuf;
			_poArrayBuf = NULL;
		}
	}
	bool init(int nElemNum)
	{
		_poArrayBuf = new CBufNode[nElemNum];

        CQueNode *poNode = NULL;
        for (int nElemNo = 0; nElemNo<nElemNum; ++nElemNo){
            poNode = new CQueNode(nElemNo);
            if (NULL == _poHead){
                _poHead = poNode;
                _poHead->ptNext = _poHead;
                _poHead->ptPrv = _poHead;
            }
            else{
                poNode->ptPrv = _poHead->ptPrv;
                poNode->ptNext = _poHead;
                _poHead->ptPrv->ptNext = poNode;
                _poHead->ptPrv = poNode;

                _poHead = poNode;
            }

            // relate with cache and page No
            _poArrayBuf[nElemNo].nPageNo = NO_PAGE;
            _poArrayBuf[nElemNo].ptNode = poNode;
        }

        return true;
    }

	bool updatePriority(unsigned short nBufNo)
	{
        CQueNode *poNode = _poArrayBuf[nBufNo].ptNode;

        if (poNode == _poHead){
            return true;
        }

        // delete from double link
        poNode->ptPrv->ptNext = poNode->ptNext;
        poNode->ptNext->ptPrv = poNode->ptPrv;

        // insert into head
        poNode->ptPrv = _poHead->ptPrv;
        poNode->ptNext = _poHead;
        _poHead->ptPrv->ptNext = poNode;
        _poHead->ptPrv = poNode;

        _poHead = poNode;

        // for debug
        _nHitTimes++;

        return true;
    }

	bool getLazyBufNoAndSwapPage(unsigned short nSwapPageNo, unsigned short& nDelPageNo, unsigned short& nBufNo)
	{
        nBufNo = (_poHead->ptPrv)->_nBufNo;
        nDelPageNo = _poArrayBuf[nBufNo].nPageNo;
        _poArrayBuf[nBufNo].nPageNo = nSwapPageNo;

        // Insert node into queue head
        _poHead = _poHead->ptPrv;

        // for debug
        _nSwapPageTimes++;

        return true;
    }

	void show(void) const
	{

	}
private:
    int _nElemNum;

    CBufNode *_poArrayBuf;
    CQueNode *_poHead;

    // for debug
    long _nSwapPageTimes;
    long _nHitTimes;
};

class CDevCache;
class CPage
{
    friend class CDevCache;
    static const int PAGE_PROTECT = 0x12345678;

public:
	CPage(int nSize) : _nPageSize(nSize)
	{
        _pcPage = new char[nSize * sizeof(char) + sizeof(PAGE_PROTECT)];
		int *pnProtect = (int *)(_pcPage + _nPageSize);
		*pnProtect = PAGE_PROTECT;
	}

	~CPage()
	{
        delete _pcPage;
		_pcPage = NULL;
	}

	const char* getPage(void) const
	{
        return _pcPage;
    }

	bool check(void) const
	{
        int *pnProtect = (int *)(_pcPage + sizeof(_nPageSize));
        return (PAGE_PROTECT == *pnProtect);
    }

private:
    char* _pcPage;
    int   _nPageSize;
};

class CDevCache
{
    static const int DEFAULT_NUM = 100;
    static const int DEV_PAGE_SIZE = 4096;

public:
    CDevCache(int nPageSize = DEV_PAGE_SIZE, 
              int nMaxBufferNum = DEFAULT_NUM);

    virtual ~CDevCache();

    bool init(const string& strFilePath);

    bool init(FILE *pFile);

    // 获取一个页面
    const char* getPageByNo(int nPageNo);

    // for test
    void show(void);

    private:
    // 加载一页
    bool downloadPage(int nBufferNo, int nPageNo);

    private:
    int _nPageSize;
    int _nMaxPageNum;
    int _nMaxBufferNum;

    CPriorityQue _Priorityque;
    CPageMap _PageMap;
    CPage **_ppBuffer;

    FILE *_pFile;

    bool _bInited;
    int _nSem;
    int _nCountSem;
};

#endif // __DEV_BUFFER_H__
