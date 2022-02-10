#ifndef __CDATA_DIAG_H__
#define __CDATA_DIAG_H__

#include "CProtolDiag.h"
#include "CModuleMgr.h"

class CDataDiag:public CProtolDiag{
    public:
        CDataDiag(CModuleMgr *lpModMgr);
        virtual ~CDataDiag();

        virtual int process(char* req, int reqlen, char* rsp, int rsplen, int& retlen);

        virtual int pre_write(char* buff, int nlen);
        virtual int post_write(char* buff, int nlen, int nsend);

    private:
        CModuleMgr* m_lpModMgr;
};

#endif