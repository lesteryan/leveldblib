
#include <iostream>
#include <stdio.h>

using std::cout;
using std::endl;

#include "dev_buffer.h"

CDevCache::CDevCache(int nPageSize, int nMaxBufferNum)
		  :_nPageSize(nPageSize), _nMaxBufferNum(nMaxBufferNum), _pFile(NULL), _bInited(false)
{
}

CDevCache::~CDevCache()
{
	for (int i=0; i<_nMaxBufferNum; i++) {
		if (NULL != _ppBuffer[i]) {
	        delete _ppBuffer[i];
		    _ppBuffer[i] = NULL;
		}
	}
	delete _ppBuffer;
	_ppBuffer = NULL;
}

bool CDevCache::init(FILE *pFile)
{
    if (_bInited){
        return true;
    }
    if (NULL == pFile)
    {
        cout<<"false to init "<<endl;
        return false;
    }
    _pFile = pFile;
    fseek(_pFile, 0, SEEK_END);
    long nFileSize = ftell(_pFile);
    int nPageNum = (nFileSize % _nPageSize == 0) ? nFileSize / _nPageSize : nFileSize / _nPageSize + 1;
    fseek(_pFile, 0, SEEK_SET);

    _nMaxPageNum = nPageNum;
    _PageMap.init(nPageNum);
    _Priorityque.init(_nMaxBufferNum);

    _ppBuffer = new CPage* [_nMaxBufferNum];
    for (int nNo=0; nNo<_nMaxBufferNum; ++nNo){
        _ppBuffer[nNo] = NULL;
    }

    return true;
}

// 加载一页
bool CDevCache::downloadPage(int nBufferNo, int nPageNo){
    if (NULL == _ppBuffer[nBufferNo]){
        _ppBuffer[nBufferNo] = new CPage(_nPageSize);
    }

    fseek(_pFile, nPageNo * _nPageSize, SEEK_SET);

    fread(_ppBuffer[nBufferNo]->_pcPage, sizeof(char), _nPageSize, _pFile);

    return true;
}

// 获取一个页面
const char* CDevCache::getPageByNo(int nPageNo){
    if (_nMaxPageNum <= nPageNo) {
        return NULL;
    }

    unsigned short nLazyPageNo = 0, nBufNo = 0;
    nBufNo = _PageMap.getBufferNo(nPageNo);
    if (CPageMap::BUFFER_NULL != nBufNo){
        _Priorityque.updatePriority(nBufNo);
        
        return _ppBuffer[nBufNo]->getPage();
    }

    // 产生缺页
    _Priorityque.getLazyBufNoAndSwapPage(nPageNo, nLazyPageNo, nBufNo);
    _PageMap.addPageMap(nPageNo, nBufNo);
    if (CPriorityQue::NO_PAGE != nLazyPageNo){
        _PageMap.delPageMap(nLazyPageNo);
    }

    downloadPage(nBufNo, nPageNo);

    return _ppBuffer[nBufNo]->getPage();
}

// for test
void CDevCache::show(void){
}

