#ifndef __CDATA_AT_H__
#define __CDATA_AT_H__

#include "CProtolAT.h"
#include "CModuleMgr.h"

class CDataAT:public CProtolAT{
    public:
        CDataAT(CModuleMgr *lpModMgr);
        virtual ~CDataAT();

        virtual int process(char* req, int reqlen, char* rsp, int rsplen, int& retlen);

    private:
        CModuleMgr* m_lpModMgr;

};

#endif
