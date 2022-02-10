#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>


#include "CTrans.h"
#include "CPort.h"
#include "CChnlThread.h"
#include "CChnlThreadMgr.h"
#include "englog.h"
#include "modules.h"

#define Info(fmt, args...) {\
        if (m_dataType != DATA_LOG) { \
            info(fmt, ##args); \
        } \
    }

CTrans::CTrans(){
}

CTrans::~CTrans(){
}

void CTrans::init(char* name, CPort* lpSrc, CPort* lpDst, int dataType, int apProcess){
    strcpy(m_name, name);
    m_lpPortSrc = lpSrc;
    m_lpPortDst = lpDst;
    m_dataType = dataType;
    m_apProcess = apProcess;
    m_retType = DYMIC_RET_NO_DEAL;
    m_frameType = FRAME_COMPLETE;
    lpSrc->attach(this);
    lpDst->attach(this);
}

void CTrans::activeOtherChnlDstPort(){
    info("activeOtherChnlDstPort: dev:%s, port: %s", m_lpPortSrc->getDevName(), m_lpPortSrc->getname());
    if(m_lpChnlThread != NULL){
        CChnlThreadMgr* mgr = m_lpChnlThread->getChnlThreadMgr();
        if (mgr != NULL){
            CChnlThread* lpChnlThread = mgr->findSameDstPort(m_lpPortSrc);
            if ( lpChnlThread != NULL){
                info("chnl thread: %s", lpChnlThread->getName());
                lpChnlThread->activeDstPort(m_lpPortSrc);
            }
        }
    }
}

int CTrans::pre_read(char* buff, int nlen){
    return 0;
}

int CTrans::read(char* buff, int nlen){
    int ret = 0;
    info("read");
    if (m_lpPortSrc != NULL){
        CTrans* ptr = m_lpPortSrc->attach(this);
        ret = m_lpPortSrc->read(buff, nlen);
        m_lpPortSrc->attach(ptr);
    }

    return ret;
}

int CTrans::post_read(char* buff, int nlen, int nrecv){
    return 0;
}

int CTrans::pre_write(char* buff, int nlen){
    return 0;
}

int CTrans::write(char* buff, int nlen){
    int ret = 0;
    if (m_lpPortSrc != NULL){
        CTrans* ptr = m_lpPortSrc->attach(this);
        ret = m_lpPortSrc->write(buff, nlen);
        m_lpPortSrc->attach(ptr);
    }

    return ret;
}

int CTrans::post_write(char*buff, int nlen, int nsend){
    return 0;
}


int CTrans::trans(){
    info("trans: ");

    if (0 != m_lpPortSrc->open()){
        error("open src fail: %s, error = %s", m_lpPortSrc->getname(), strerror(errno));
        return -1;
    }

    if (0 != m_lpPortDst->open()){
        error("open dst fail: %s, error = %s", m_lpPortDst->getname(), strerror(errno));
        //m_lpPortSrc->close();
        //return -1;
    }

    int r_cnt = 0, w_cnt = 0;;
    int ret = DYMIC_RET_NO_DEAL;
    int retlen = 0;

    while(1){
        r_cnt = 0;
        w_cnt = 0;

        memset(m_chnl_buff_req, 0, sizeof(m_chnl_buff_req));
        memset(m_chnl_buff_rsp, 0, sizeof(m_chnl_buff_rsp));
        memset(m_chnl_buff_req_bk, 0, sizeof(m_chnl_buff_req_bk));

        // pre read
        //info("pre read");
        if (0 != pre_read(m_chnl_buff_req, sizeof(m_chnl_buff_req))){
            error("pre read return !0, break");
            continue;
        }
        // read from src
        //Info("read...");
        CTrans* ptr = m_lpPortSrc->attach(this);
        r_cnt = m_lpPortSrc->read(m_chnl_buff_req, sizeof(m_chnl_buff_req));
        m_lpPortSrc->attach(ptr);
        activeOtherChnlDstPort();
        Info("r_cnt = %d", r_cnt);
        
        //info("post read");
        if (0 != post_read(m_chnl_buff_req, sizeof(m_chnl_buff_req), r_cnt)){
            error("post read return !0, break");
            continue;
        }

        if(r_cnt <= 0){
            error("read fail, break");
            break;
        }

        //bankup req
        memcpy(m_chnl_buff_req_bk, m_chnl_buff_req, r_cnt);

        Info("process: need ap process: %d", m_apProcess);
        if ( (m_frameType == FRAME_COMPLETE || m_frameType == FRAME_HALF_CONTINUE) && m_apProcess == 1 ){
            // decode
            Info("decode...");
            // UNISOC: add for bug1358053, BT4.0 LE could not connect to the signaling table.
            if(m_dataType == DATA_WCN_AT){
                m_nLenReq = r_cnt;
            }else {
                m_nLenReq = decode(m_chnl_buff_req, r_cnt);
            }
            Info("m_nLenReq = %d", m_nLenReq);

            // ap process
            ret = process(m_chnl_buff_req, m_nLenReq, m_chnl_buff_rsp, sizeof(m_chnl_buff_rsp), retlen);
        }

        m_retType = ret;
        Info("ret = %d", ret);

        if (DYMIC_RET_NO_RESPONSE_TO_PC == ret){
            info("DYMIC_RET_NO_RESPONSE_TO_PC: continue to read next req");
            continue;
        }

        if (DYMIC_RET_DEAL_SUCCESS == ret || DYMIC_RET_ALSO_NEED_TO_CP == ret || DYMIC_RET_ENCODE_TO_DEST == ret){
            info("DYMIC_RET_DEAL_SUCCESS: write to src port");

            // encode
            info("encode...");
             // UNISOC: add for bug1358053, BT4.0 LE could not connect to the signaling table.
            if(m_dataType == DATA_WCN_AT){
                m_nLenRsp = retlen;
            }else{
                m_nLenRsp = encode(m_chnl_buff_rsp, retlen);
            }
            info("m_nLenRsp = %d", m_nLenRsp);

            if ( 0 != pre_write(m_chnl_buff_rsp, m_nLenRsp) ){
                error("pre write return !0, continue");
                continue;
            }

            if (DYMIC_RET_ENCODE_TO_DEST == ret){
                CTrans* ptr = m_lpPortDst->attach(this);
                w_cnt = m_lpPortDst->write(m_chnl_buff_rsp, m_nLenRsp);
                m_lpPortDst->attach(ptr);
                Info("w_cnt = %d", w_cnt);
            }else{
                CTrans* ptr = m_lpPortSrc->attach(this);
                w_cnt = m_lpPortSrc->write(m_chnl_buff_rsp, m_nLenRsp);
                m_lpPortSrc->attach(ptr);
                info("w_cnt = %d", w_cnt);
            }

            if ( 0 != post_write(m_chnl_buff_rsp, m_nLenRsp, w_cnt)){
                error("post write return !0, continue");
                continue;
            }

            if (w_cnt < m_nLenRsp){
                error("write fail, continue");
            }

            if (DYMIC_RET_DEAL_SUCCESS == ret) {
                continue;
            }
        }

        if( DYMIC_RET_NO_DEAL == ret || DYMIC_RET_ALSO_NEED_TO_CP == ret ){
            // write to dst
            Info("write...: portType = %s", m_lpPortDst->getPortTypeStr());
            if(m_lpPortDst->getPortType() != PORT_LOOP){

                if ( 0 != pre_write(m_chnl_buff_req_bk, r_cnt) ){
                    error("pre write return !0, continue");
                    continue;
                }

                CTrans* ptr = m_lpPortDst->attach(this);
                w_cnt = m_lpPortDst->write(m_chnl_buff_req_bk, r_cnt);
                m_lpPortDst->attach(ptr);
                Info("w_cnt = %d", w_cnt);

                if ( 0 != post_write(m_chnl_buff_req_bk, r_cnt, w_cnt)){
                    error("post write return !0, continue");
                    continue;
                }

                if (w_cnt < r_cnt){
                    error("write fail, continue");
                }
            }
        }
    }

    m_lpPortSrc->close();
    m_lpPortDst->close();

    return -1;
}

int CTrans::decode(char* buff, int nlen){
    return nlen;
}

int CTrans::encode(char* buff, int nlen){
    return nlen;
}

FRAME_TYPE CTrans::checkframe(char* buff, int& nlen){
    return FRAME_COMPLETE;
}

int CTrans::findframe(char* buff, int nlen){
    return nlen;
}

int CTrans::process(char* req, int reqlen, char* rsp, int rsplen, int& retlen){
    if (retlen > 0){
        return DYMIC_RET_DEAL_SUCCESS;
    }else if (retlen  == ENG_DIAG_NO_RESPONSE){
        return DYMIC_RET_NO_RESPONSE_TO_PC;
    }else{
        return DYMIC_RET_NO_DEAL;
    }
}

#define LOG_BUF_SIZE 1024

int CTrans::info(const char* fmt, ...){
    va_list ap;
    char buf[LOG_BUF_SIZE];
    
    va_start(ap, fmt);
    vsnprintf(buf, LOG_BUF_SIZE, fmt, ap);
    va_end(ap);

    return EngLog::info("%s %s", TAG(), buf);
}

int CTrans::debug(const char* fmt, ...){
    va_list ap;
    char buf[LOG_BUF_SIZE];
    
    va_start(ap, fmt);
    vsnprintf(buf, LOG_BUF_SIZE, fmt, ap);
    va_end(ap);

    return EngLog::debug("%s %s", TAG(), buf);
}

int CTrans::warn(const char* fmt, ...){
    va_list ap;
    char buf[LOG_BUF_SIZE];
    
    va_start(ap, fmt);
    vsnprintf(buf, LOG_BUF_SIZE, fmt, ap);
    va_end(ap);

    return EngLog::warn("%s %s", TAG(), buf);
}

int CTrans::error(const char* fmt, ...){
    va_list ap;
    char buf[LOG_BUF_SIZE];
    
    va_start(ap, fmt);
    vsnprintf(buf, LOG_BUF_SIZE, fmt, ap);
    va_end(ap);

    return EngLog::error("%s %s", TAG(), buf);
}