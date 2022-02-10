#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "CProtolAT.h"

CProtolAT::CProtolAT(){
}

CProtolAT::~CProtolAT(){
}

int CProtolAT::decode(char* buff, int len){
    info("CProtolAT decode: buff = %s, len = %d", buff, len);
    return strlen(buff);
}

int CProtolAT::encode(char* buff, int len){
    info("CProtolAT encode: buff = %s, len = %d", buff, len);
    return strlen(buff);
}

int CProtolAT::process(char* req, int reqlen, char* rsp, int rsplen, int& retlen){
    if (retlen == 0 && strlen(rsp) > 0){
        retlen = strlen(rsp);
    }

    return 0;
}
