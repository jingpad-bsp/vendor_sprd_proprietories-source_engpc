#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "CDataAT.h"
#include "CProtolAT.h"

CDataAT::CDataAT(CModuleMgr *lpModMgr){
    m_lpModMgr = lpModMgr;
}

CDataAT::~CDataAT(){
}

int CDataAT::process(char* req, int reqlen, char* rsp, int rsplen, int& retlen){
    int ret = DYMIC_RET_NO_DEAL;
    int cp_process = 0;
    info("process:");
    if (m_lpModMgr != NULL){
        retlen = m_lpModMgr->process((DATA_TYPE)m_dataType, req, reqlen, rsp, rsplen, cp_process);
    }

    //some so process diag with at successfully  but return 0, so we must recheck.
    CProtolAT::process(req, reqlen, rsp, rsplen, retlen);

    ret = CTrans::process(req, reqlen, rsp, rsplen, retlen);
    if (DYMIC_RET_DEAL_SUCCESS == ret && cp_process == 1){
        ret = DYMIC_RET_ALSO_NEED_TO_CP;
    }

    return ret;
}

