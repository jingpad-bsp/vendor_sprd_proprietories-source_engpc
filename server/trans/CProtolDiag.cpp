#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "CProtolDiag.h"
#include "modules.h"

CProtolDiag::CProtolDiag(){
    m_nDiagAT = 0;
    m_bPending = false;
    m_iCurFrameType = FRAME_INVALID;
}

CProtolDiag::~CProtolDiag(){
}

int CProtolDiag::decode(char* buff, int len){
    int i, j, retlen;
    unsigned char tmp;

    info("%s: len=%d", __FUNCTION__, len);

    retlen = len;
    for (i = 1; i < len-2; i++) {
        if ((buff[i] == 0x7d) || (buff[i] == 0x7e)) {
            tmp = buff[i + 1] ^ 0x20;
            buff[i] = tmp;
            for(j = i + 1; j < len; j ++){
                buff[j] = buff[j+1];
            }
            len--;
            retlen--;
        }
    
    }

    info("%s: retlen=%d", __FUNCTION__, retlen);

    m_nDiagAT = (buff[7] == 0x68)?1:0;

    return retlen;
}

int CProtolDiag::encode(char* buff, int len){
    int i, j, retlen;
    char tmp;

    info("%s: len=%d", __FUNCTION__, len);

    if ((m_retType == DYMIC_RET_DEAL_SUCCESS || m_retType == DYMIC_RET_ALSO_NEED_TO_CP) && m_nDiagAT){
        do {
            MSG_HEAD_T msg_head_ptr = {0};
            int nPendingLen = 0;
            if (m_bPending) nPendingLen = strlen(PENDING_MARK);

            msg_head_ptr.seq_num = 0;
            msg_head_ptr.type = 0x9c;
            msg_head_ptr.subtype = 0x00;
            len = strlen(buff) + sizeof(MSG_HEAD_T)-nPendingLen;
            memmove(buff+sizeof(MSG_HEAD_T)+1, buff, strlen(buff)-nPendingLen);
            memcpy(buff+1, &msg_head_ptr, sizeof(MSG_HEAD_T));
            ((MSG_HEAD_T *)(buff+1))->len = len;
            buff[0] = 0x7E;
            buff[len+1] = 0x7E;
            len += 2;
        } while (0);

        info("%s: new len=%d", __FUNCTION__, len);
    }

    retlen = len;
    for (i = 1; i < len-1; i++) {
        if ((buff[i] == 0x7d) || (buff[i] == 0x7e)) {
            tmp = buff[i] ^ 0x20;
            info("%s: tmp=%x, buf[%d]=%x", __FUNCTION__, tmp, i, buff[i]);
            buff[i] = 0x7d;
            for (j = len; j > i + 1; j--) {
                buff[j] = buff[j - 1];
            }
            buff[i + 1] = tmp;
            len++;
            retlen++;
        }
    }

    info("%s: retlen=%d", __FUNCTION__, retlen);
    return retlen;
}

/*
1.  7E XX XX XX ... 7E :  compete frame, ap process or cp process
2.  XX XX XX XX ... XX :  invalid frame, send to cp
3.  7E XX XX XX ... XX :  invalid frame, ap process or cp process

other:
1. XX XX ... XX 7E XX XX XX ... 7E XX XX : find frame (7E XX XX ... XX XX 7E)
2. XX XX ... XX 7E XX XX XX ... XX XX XX : find frame (7E XX XX ... XX XX XX)
*/

FRAME_TYPE CProtolDiag::checkframe(char* buff, int& nlen){
    if (getApProcess() != 1){
        return FRAME_COMPLETE;
    }

    FRAME_TYPE ret = FRAME_INVALID;
    if (buff[0] == 0x7E && buff[nlen-1] == 0x7E){
        ret = FRAME_COMPLETE;
    }else {
        int start = findFrameFlag(buff, nlen);
        if (start != -1){
            int end = findFrameFlag(buff+start+1, nlen-start-1);
            if (end != -1){
                memmove(buff, buff+start, end+2);
                nlen = end+2;
                ret = FRAME_COMPLETE;
            }else{
                memmove(buff, buff+start, nlen-start);
                nlen = nlen-start;
                ret = FRAME_HALF_CONTINUE;
            }
        }
    }

    m_iCurFrameType = ret;

    return ret;
}

int CProtolDiag::findframe(char* buff, int nlen){
    return nlen;
}

int CProtolDiag::findFrameFlag(char* buff, int nlen){
    int offset = 0;
    char* ptr = buff;
    if (ptr == NULL) return -1;
    while(offset < nlen){
        if(*(ptr++) == 0x7E){
            break;
        }else{
            offset++;
        }
    }

    if (offset >= nlen){
        offset = -1;
    }

    return offset;
}

bool CProtolDiag::checkPending(char*rsp, int nlen){
    int ret = 0;

    if (m_nDiagAT){
        int at_ret_error = ((strcasestr(rsp, "ERROR")) != NULL) ? 1 : 0;

        // whether rsp contains "\r\nPENDING\r\n"
        if (!at_ret_error && (strcasestr(rsp, PENDING_MARK)) != NULL) {
            int pending_mark_len = strlen(PENDING_MARK);
            if (0 == strncmp(rsp + strlen(rsp) - pending_mark_len, PENDING_MARK, pending_mark_len)) {
                return 1;
            }
        }
    }

    return 0;
}

int CProtolDiag::process(char* req, int reqlen, char* rsp, int rsplen, int& retlen){
    if (m_nDiagAT && retlen == 0 && strlen(rsp) > 0){
        retlen = strlen(rsp);
    }

    return 0;
}


int CProtolDiag::translate_packet(char *dest, char *src, int size){
    int i;
    int translated_size = 0;

    dest[translated_size++] = 0x7E;

    for (i = 0; i < size; i++) {
        if (src[i] == 0x7E) {
            dest[translated_size++] = 0x7D;
            dest[translated_size++] = 0x5E;
        }else if (src[i] == 0x7D) {
            dest[translated_size++] = 0x7D;
            dest[translated_size++] = 0x5D;
        }else {
            dest[translated_size++] = src[i];
        }
    }

    dest[translated_size++] = 0x7E;
    return translated_size;
}
