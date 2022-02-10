#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "CDataAgdsp.h"

CDataAgdsp::CDataAgdsp(CModuleMgr *lpModMgr){
    m_lpModMgr = lpModMgr;
}

CDataAgdsp::~CDataAgdsp(){
}

int CDataAgdsp::process(char* req, int reqlen, char* rsp, int rsplen, int& retlen){
    int ret = DYMIC_RET_NO_DEAL;
    info("process:");

    //return deal success, so ctrans can encode
    memcpy(rsp, req, reqlen);
    retlen = reqlen;
    ret = DYMIC_RET_ENCODE_TO_DEST;

    return ret;
}

