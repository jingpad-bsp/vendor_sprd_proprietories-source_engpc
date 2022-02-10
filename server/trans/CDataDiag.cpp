#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "CDataDiag.h"

CDataDiag::CDataDiag(CModuleMgr *lpModMgr){
    m_lpModMgr = lpModMgr;
}

CDataDiag::~CDataDiag(){
}

int CDataDiag::process(char* req, int reqlen, char* rsp, int rsplen, int& retlen){
    int ret = DYMIC_RET_NO_DEAL;
    int cp_process = 0;
    if (m_lpModMgr != NULL){
        retlen = m_lpModMgr->process((DATA_TYPE)m_dataType, req, reqlen, rsp, rsplen, cp_process);
    }

    //some so process diag with at successfully  but return 0, so we must recheck.
    CProtolDiag::process(req, reqlen, rsp, rsplen, retlen);

    ret = CTrans::process(req, reqlen, rsp, rsplen, retlen);
    if (DYMIC_RET_DEAL_SUCCESS == ret && cp_process == 1){
        ret = DYMIC_RET_ALSO_NEED_TO_CP;
    }

    if (m_lpModMgr == NULL){
        return ret;
    }
    if (checkPending(rsp, rsplen)){
        m_lpModMgr->setPendingMark();
        setPendingMark(true);
    }else{
        m_lpModMgr->clearPendingMark();
        setPendingMark(false);
    }

    return ret;
}

char emptyDiag[] = {0x7e, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0xd5, 0x00, 0x7e};
char okRsp[] = {0x7E, 0x00, 0x00, 0x00, 0x00, 0x0E, 0x00, 0x9C, 0x00, 0x0D, 0x0A, 0x4F, 0x4B, 0x0D, 0x0A, 0x7E};

int CDataDiag::pre_write(char* buff, int nlen){
    info("pre_write");
    if ((m_retType == DYMIC_RET_DEAL_SUCCESS || m_retType == DYMIC_RET_ALSO_NEED_TO_CP) && m_nDiagAT){
        write(emptyDiag, sizeof(emptyDiag));
    }

    return 0;
}

int CDataDiag::post_write(char* buff, int nlen, int nsend){
    info("post_write");
    if ((m_retType == DYMIC_RET_DEAL_SUCCESS || m_retType == DYMIC_RET_ALSO_NEED_TO_CP) && m_nDiagAT){
        if (strcasecmp(buff, "\r\nOK")){
            write(okRsp, sizeof(okRsp));
        }
    }

    return 0;
}


