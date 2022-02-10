#ifndef __CDATA_AGDSP_H__
#define __CDATA_AGDSP_H__

#include "CProtolSmp.h"
#include "CModuleMgr.h"

class CDataAgdsp:public CProtolSmp{
    public:
        CDataAgdsp(CModuleMgr *lpModMgr);
        virtual ~CDataAgdsp();

        virtual int process(char* req, int reqlen, char* rsp, int rsplen, int& retlen);

    private:
        CModuleMgr* m_lpModMgr;
};

#endif