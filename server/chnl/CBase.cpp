#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "CBase.h"
#include "englog.h"
#include "channel.h"

#define stricmp strcasecmp

CBase::CBase(){

}

CBase::~CBase(){

}

PORT_TYPE CBase::str2PortType(char* str){
    if(0 == stricmp(str, "VSER")){
        return PORT_VSER;
    }if(0 == stricmp(str, "GSER")){
        return PORT_GSER;
    }else if(0 == stricmp(str, "SOCK")){
        return PORT_SOCK;
    }else if(0 == stricmp(str, "COM")){
        return PORT_COM;
    }else if(0 == stricmp(str, "CHR")){
        return PORT_CHR;
    }else if(0 == stricmp(str, "INTERFACE")){
        return PORT_INTERFACE;
    }else if(0 == stricmp(str, "LOOP")){
        return PORT_LOOP;
    }else{
        return PORT_UNKNOWN;
    }
}

char* CBase::PortType2str(PORT_TYPE type){
    if (type == PORT_VSER){
        return "VSER";
    }else if (type == PORT_GSER){
        return "GSER";
    }else if (type == PORT_SOCK){
        return "SOCK";
    }else if (type == PORT_COM){
        return "COM";
    }else if (type == PORT_CHR){
        return "CHR";
    }else if (type == PORT_INTERFACE){
        return "INTERFACE";
    }else if (type == PORT_LOOP){
        return "LOOP";
    }else {
        return "PORT_UNKNOWN";
    }
}


DATA_TYPE CBase::str2DataType(char* str){
    if(0 == stricmp(str, "DATA_DIAG")){
        return DATA_DIAG;
    }else if(0 == stricmp(str, "DATA_AT")){
        return DATA_AT;
    }else if(0 == stricmp(str, "DATA_LOG")){
        return DATA_LOG;
    }else if(0 == stricmp(str, "DATA_WCN_DIAG")){
        return DATA_WCN_DIAG;
    }else if(0 == stricmp(str, "DATA_WCN_AT")){
        return DATA_WCN_AT;
    }else if(0 == stricmp(str, "DATA_AGDSP_LOG")){
        return DATA_AGDSP_LOG;
    }else if(0 == stricmp(str, "DATA_AGDSP_PCM")){
        return DATA_AGDSP_PCM;
    }else if(0 == stricmp(str, "DATA_AGDSP_MEM")){
        return DATA_AGDSP_MEM;
    }else if(0 == stricmp(str, "DATA_AGDSP_MIX")){
        return DATA_AGDSP_MIX;
    }else{
        return DATA_TYPE_UNKNOWN;
    }
}

char* CBase::DataType2str(DATA_TYPE type){
    if (type == DATA_DIAG){
        return "DATA_DIAG";
    }else if (type == DATA_AT){
        return "DATA_AT";
    }else if (type == DATA_LOG){
        return "DATA_LOG";
    }else if (type == DATA_WCN_DIAG){
        return "DATA_WCN_DIAG";
    }else if (type == DATA_WCN_AT){
        return "DATA_WCN_AT";
    }else if (type == DATA_AGDSP_LOG){
        return "DATA_AGDSP_LOG";
    }else if (type == DATA_AGDSP_PCM){
        return "DATA_AGDSP_PCM";
    }else if (type == DATA_AGDSP_MEM){
        return "DATA_AGDSP_MEM";
    }else if (type == DATA_AGDSP_MIX){
        return "DATA_AGDSP_MIX";
    }else {
        return "DATA_TYPE_UNKNOWN";
    }
}

int CBase::str2act(char* act){
    if (0 == stricmp(act, "F")){
        return WORK_MODE_F;
    }else if (0 == stricmp(act, "S")){
        return WORK_MODE_S;
    }else{
        return WORK_MODE_UNKNOW;
    }
}

char* CBase::act2str(int mode){
    if (mode == WORK_MODE_F){
        return "F";
    }else if (mode == WORK_MODE_S){
        return "S";
    }else{
        return "unknown";
    }
}

int CBase::str2enable(char* enable){
    if (enable == NULL){
        return 0;
    }

    int value = atoi(enable);
    if (value > 0){
        return 1;
    }else{
        return 0;
    }
}

int CBase::str2approcess(char* ap_process){
    return str2enable(ap_process);
}

#define LOG_BUF_SIZE 1024

int CBase::info(const char* fmt, ...){
    va_list ap;
    char buf[LOG_BUF_SIZE];
    
    va_start(ap, fmt);
    vsnprintf(buf, LOG_BUF_SIZE, fmt, ap);
    va_end(ap);

    return EngLog::info("%s %s", TAG(), buf);
}

int CBase::debug(const char* fmt, ...){
    va_list ap;
    char buf[LOG_BUF_SIZE];
    
    va_start(ap, fmt);
    vsnprintf(buf, LOG_BUF_SIZE, fmt, ap);
    va_end(ap);

    return EngLog::debug("%s %s", TAG(), buf);
}

int CBase::warn(const char* fmt, ...){
    va_list ap;
    char buf[LOG_BUF_SIZE];
    
    va_start(ap, fmt);
    vsnprintf(buf, LOG_BUF_SIZE, fmt, ap);
    va_end(ap);

    return EngLog::warn("%s %s", TAG(), buf);
}

int CBase::error(const char* fmt, ...){
    va_list ap;
    char buf[LOG_BUF_SIZE];
    
    va_start(ap, fmt);
    vsnprintf(buf, LOG_BUF_SIZE, fmt, ap);
    va_end(ap);

    return EngLog::error("%s %s", TAG(), buf);
}
