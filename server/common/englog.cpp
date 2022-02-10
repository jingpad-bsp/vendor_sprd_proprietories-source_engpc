#include <stdio.h>
#include <stdlib.h>
#include <log/log.h>

#include "englog.h"
#include "sprd_fts_log.h"
#include "sprd_fts_type.h"
#include "adapter.h"

EngLog::EngLog(){

}

EngLog::~EngLog(){

}

#define AT_ENGPCLOGLEVEL "AT+SPENGPCLOGLEVEL"

void EngLog::regCallBack(CModuleMgr* pModMgr){
    struct eng_callback cb = {0};
    cb.diag_ap_cmd = -1;
    cb.type = 0xFF;
    cb.subtype = 0xFF;
    sprintf(cb.at_cmd, "%s", AT_ENGPCLOGLEVEL);
    cb.eng_linuxcmd_func = logLevelHandle;

    pModMgr->internalRegCallBack(&cb, 1);
}

int EngLog::logLevelHandle(char* buff, char* rsp){
    char *ptr = NULL;
    char cmd_buf[256] = {0};
    int ret = -1;
    int nlen = 0;
    if(NULL == rsp){
        return 0;
    }else if(NULL == buff){
        return sprintf(rsp, "\r\nERROR\r\n");
    }

    if(buff[0] == 0x7e)
    {
        ptr = buff + 1 + sizeof(MSG_HEAD_T);
    }
    else
    {
        ptr = strdup(buff);
    }

    if (strncasecmp(ptr, AT_ENGPCLOGLEVEL,strlen(AT_ENGPCLOGLEVEL)) == 0){
        char *ptrpara = ptr+strlen(AT_ENGPCLOGLEVEL);
        if (ptrpara != NULL){
            if (*ptrpara == '='){
                ptrpara++;
                if (*ptrpara != NULL){
                    s_loglevel = (ENGPC_LOGLEVEL)(*ptrpara-'0');
                    sprintf(rsp, "\r\n+SPENGPCLOGLEVEL: %d\r\n", s_loglevel);
                }else{
                    sprintf(rsp, "\r\n+SPENGPCLOGLEVEL: invalid param\r\n");
                }
            }else if (*ptrpara == '?'){
                sprintf(rsp, "\r\n+SPENGPCLOGLEVEL: %d\r\n", s_loglevel);
            }else{
                sprintf(rsp, "\r\n+SPENGPCLOGLEVEL: unknown at\r\n");
            }
        }
    }else{
        sprintf(rsp, "\r\n+SPENGPCLOGLEVEL: unknown at\r\n");
    }

    if(buff[0] != 0x7e){
        free(ptr);
    }

    return strlen(rsp);

}


ENGPC_LOGLEVEL EngLog::s_loglevel = LOG_INFO;

#define LOG_BUF_SIZE 1024

int EngLog::info(const char* fmt, ...){
    va_list ap;
    char buf[LOG_BUF_SIZE];
    time_t tm = {0};

    if(s_loglevel < LOG_INFO) return 0;

    va_start(ap, fmt);
    vsnprintf(buf, LOG_BUF_SIZE, fmt, ap);
    va_end(ap);

    ENG_LOG(" [tid:%d]%s\r\n", sys_gettid(), buf);
    return 0;
}

int EngLog::debug(const char* fmt, ...){
    va_list ap;
    char buf[LOG_BUF_SIZE];
    time_t tm = {0};

    if(s_loglevel < LOG_DEBUG) return 0;

    va_start(ap, fmt);
    vsnprintf(buf, LOG_BUF_SIZE, fmt, ap);
    va_end(ap);

    ENG_LOG("[tid:%d]%s\r\n", sys_gettid(), buf);
    return 0;
}

int EngLog::warn(const char* fmt, ...){
    va_list ap;
    char buf[LOG_BUF_SIZE];
    time_t tm = {0};

    if(s_loglevel < LOG_WARN) return 0;

    va_start(ap, fmt);
    vsnprintf(buf, LOG_BUF_SIZE, fmt, ap);
    va_end(ap);

    ENG_LOG("[tid:%d]%s\r\n", sys_gettid(), buf);
    return 0;
}

int EngLog::error(const char* fmt, ...){
    va_list ap;
    char buf[LOG_BUF_SIZE];
    time_t tm = {0};

    if(s_loglevel < LOG_ERR) return 0;

    va_start(ap, fmt);
    vsnprintf(buf, LOG_BUF_SIZE, fmt, ap);
    va_end(ap);

    ENG_LOG("[tid:%d]%s\r\n", sys_gettid(), buf);
    return 0;
}

