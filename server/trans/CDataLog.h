#ifndef __CDATA_LOG_H__
#define __CDATA_LOG_H__

#include "CProtol.h"
#include "CModuleMgr.h"

class CDataLog:public CProtol{
    public:
        CDataLog(CModuleMgr *lpModMgr);
        virtual ~CDataLog();

        virtual int process(char* req, int reqlen, char* rsp, int rsplen, int& retlen);

    private:
        CModuleMgr* m_lpModMgr;

};

#endif