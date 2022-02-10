#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include <getopt.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/types.h>

#include "crc16.h"
#include "sprd_fts_diag.h"
#include "sprd_fts_log.h"
#include "phasecheck.h"
#include "translate_packet.h"

int direct_phschk(char *buf, int len, char *rsp, int rsplen) {
    int crc = 0;
    int data_len = 0;
    int recv_crc = 0;
    int rsp_len = 0;
    unsigned char result;
    unsigned char *nvdata;
    ERR_IMEI_E error;
    uint32_t magic;
    int sn_len = 0;

    NVITEM_ERROR_E nverr = NVERR_NONE;
    MSG_HEAD_T *msg_head = (MSG_HEAD_T *)(buf + 1);
    
    do {
        recv_crc =
            *(unsigned short *)&(buf[msg_head->len - sizeof(unsigned short) + 1]);
        crc = crc16(0, (unsigned char *)(msg_head + 1),
                    msg_head->len - sizeof(MSG_HEAD_T) - sizeof(unsigned short));

        if (recv_crc != crc) {
            ENG_LOG("%s: CRC Error! recv_crc: %d, crc16: %d\n", __FUNCTION__,
                  recv_crc, crc);
            msg_head->len = sizeof(MSG_HEAD_T) + 2 * sizeof(unsigned short);
            *(unsigned short *)(msg_head + 1) = IMEI_CRC_ERR;
            break;
        }

        ENG_LOG("%s: Current oper: %d\n", __FUNCTION__,
            (msg_head->subtype & RW_MASK));

        if ((msg_head->subtype & RW_MASK) == WRITE_MODE) {
            if (0 != (msg_head->subtype & RM_VALID_CMD_MASK)) {
                nvdata = (unsigned char *)(msg_head + 1);
                data_len = msg_head->len - sizeof(MSG_HEAD_T) - sizeof(unsigned short);
                
                ENG_LOG("%s: data_len: %d\n", __FUNCTION__, data_len);
                
                nverr = eng_write_miscdata_with_offset(0, nvdata, data_len);
                if (NVERR_NONE != nverr) {
                    ENG_LOG("%s:Write ERROR: %d\n", __FUNCTION__, nverr);
                    error = IMEI_SAVE_ERR;
                    result = 0;
                } else {
                    error = IMEI_ERR_NONE;
                    result = 1;
                }
            } else {
                ENG_LOG("%s: Write error, subtype : %d\n", __FUNCTION__,
                        msg_head->subtype);
                error = IMEI_CMD_ERR;
                result = 0;
            }

            if (result) {
                msg_head->len = sizeof(MSG_HEAD_T) + sizeof(unsigned short);
                msg_head->subtype = MSG_ACK;
                *((unsigned short *)((unsigned char *)(msg_head + 1))) = 0;
            } else {
                msg_head->subtype = MSG_NACK;
                *(unsigned short *)(msg_head + 1) = error;
                *((unsigned short *)((unsigned char *)(msg_head + 1) +
                                     sizeof(unsigned short))) = 0;
                msg_head->len = sizeof(MSG_HEAD_T) + 2 * sizeof(unsigned short);
            }

            rsp_len = translate_packet(rsp, (unsigned char *)msg_head, msg_head->len);
        } else {  // Read Mode
            ENG_LOG("%s: Read Mode ! \n", __FUNCTION__);
            nvdata = (unsigned char *)malloc(rsplen + sizeof(MSG_HEAD_T) +
                                           sizeof(unsigned short));
            memcpy(nvdata, msg_head, sizeof(MSG_HEAD_T));
            
            nverr = eng_read_miscdata_with_offset(0, nvdata + sizeof(MSG_HEAD_T), rsplen);
            if (NVERR_NONE != nverr) {
                ENG_LOG("%s:Read ERROR: %d\n", __FUNCTION__, nverr);
                msg_head->len = sizeof(MSG_HEAD_T) + sizeof(unsigned short);
                *((unsigned short *)((unsigned char *)(msg_head + 1))) = 0;
                msg_head->subtype = MSG_NACK;
            } else {
                msg_head = (MSG_HEAD_T *)nvdata;
                magic = *((uint32_t*)(nvdata + sizeof(MSG_HEAD_T)));
                ENG_LOG("%s:phase check magic: %x\n", __FUNCTION__,magic);
                if(magic == SP09_SPPH_MAGIC_NUMBER){
                    sn_len = sizeof(SP09_TEST_DATA_INFO_T);
                }else if(magic == SP15_SPPH_MAGIC_NUMBER){
                    sn_len = sizeof(SP15_TEST_DATA_INFO_T);
                }
                msg_head->len = sizeof(MSG_HEAD_T) + sn_len +
                        sizeof(unsigned short);

                *((unsigned short *)((unsigned char *)(msg_head + 1) + sn_len)) =
                    crc16(0, ((unsigned char *)(msg_head + 1)), sn_len);

                msg_head->subtype = MSG_ACK;
            }

            rsp_len = translate_packet(rsp, (unsigned char *)msg_head, msg_head->len);

            free(nvdata);
        }
    } while (0);

  ENG_LOG("%s: rsp_len : %d\n", __FUNCTION__, rsp_len);

  return rsp_len;
}
