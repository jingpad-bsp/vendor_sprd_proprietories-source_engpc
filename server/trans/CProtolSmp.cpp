#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "CProtolSmp.h"

CProtolSmp::CProtolSmp(){
    m_sn = 0;
}

CProtolSmp::~CProtolSmp(){
}

int CProtolSmp::decode(char* buff, int nlen){
    info("decode");
    return nlen;
}

int CProtolSmp::encode(char* buff, int nlen){
    info("encode: type = %d", m_dataType);
    memmove(buff+20, buff, nlen);
    if (m_dataType == DATA_AGDSP_LOG){
        ag_dsplog_add_headers((uint8_t*)buff, nlen, AUDIO_DSP_LOG);
    }else if (m_dataType == DATA_AGDSP_PCM){
        ag_dsplog_add_headers((uint8_t*)buff, nlen, AUDIO_DSP_PCM);
    }else if (m_dataType == DATA_AGDSP_MEM){
        ag_dsplog_add_headers((uint8_t*)buff, nlen, AUDIO_DSP_MEM);
    }else{
        return nlen;
    }

    return nlen+20;
}

// Add SMP and message header
void CProtolSmp::ag_dsplog_add_headers(uint8_t* buf, size_t len, unsigned type) {
    // SMP header first

    // FLAGS
    buf[0] = 0x7e;
    buf[1] = 0x7e;
    buf[2] = 0x7e;
    buf[3] = 0x7e;
    // LEN (length excluding FLAGS in little Endian)
    size_t pkt_len = len + 8 + 8;
    buf[4] = (uint8_t)(pkt_len);
    buf[5] = (uint8_t)(pkt_len >> 8);
    // CHANNEL
    buf[6] = 0;
    // TYPE
    buf[7] = 0;
    // RESERVED
    buf[8] = 0x5a;
    buf[9] = 0x5a;
    // Checksum (only cover LEN, CHANNEL, TYPE and RESERVED)
    uint32_t n = (uint32_t)(pkt_len) + 0x5a5a;
    n = (n & 0xffff) + (n >> 16);
    n = ~n;
    buf[10] = (uint8_t)(n);
    buf[11] = (uint8_t)(n >> 8);

    // MSG_HEAD_T

    // SN
    buf[12] = (uint8_t)(m_sn);
    buf[13] = (uint8_t)(m_sn >> 8);
    buf[14] = (uint8_t)(m_sn >> 16);
    buf[15] = (uint8_t)(m_sn >> 24);
    ++m_sn;

    // length
    pkt_len = len + 8;
    buf[16] = (uint8_t)(pkt_len);
    buf[17] = (uint8_t)(pkt_len >> 8);
    // type
    buf[18] = 0x9d;
    // subtype: AG-DSP
    buf[19] = (uint8_t)(0x40 | type);
}
