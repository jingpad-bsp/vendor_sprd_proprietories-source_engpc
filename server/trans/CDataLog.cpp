#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "CDataLog.h"

CDataLog::CDataLog(CModuleMgr *lpModMgr){
    m_lpModMgr = lpModMgr;
}

CDataLog::~CDataLog(){
}

int CDataLog::process(char* req, int reqlen, char* rsp, int rsplen, int& retlen){
    int ret = DYMIC_RET_NO_DEAL;
    int cp_process = 0;
    info("process:");
    if (m_lpModMgr != NULL){
        retlen = m_lpModMgr->process((DATA_TYPE)m_dataType, req, reqlen, rsp, rsplen, cp_process);
    }

    ret = CTrans::process(req, reqlen, rsp, rsplen, retlen);
    if (DYMIC_RET_DEAL_SUCCESS == ret && cp_process == 1){
        ret = DYMIC_RET_ALSO_NEED_TO_CP;
    }

    return ret;
}

