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

#include "sprd_fts_type.h"
#include "sprd_fts_diag.h"
#include "sprd_fts_log.h"

#include "bt_wifi_mac.h"

static DYMIC_WRITETOPC_FUNC g_func[WRITE_TO_MAX] = {NULL};

int get_fw_ptr(DYMIC_WRITETOPC_FUNC * write_interface_ptr){
    ENG_LOG("%s", __FUNCTION__);

    for(int i = 0; i < WRITE_TO_MAX; i++) {
        g_func[i] = write_interface_ptr[i];
        if(g_func[i] != NULL)
            ENG_LOG("%s ad 0x%x, i %d", __FUNCTION__, g_func[i], i);
    }

    return 0;
}

int bt_wifi_mac_handler(char *buff, int len, char *rsp, int rsplen){
    int ret = btwifi_checkValid(buff);
    int retLen = 0;
    if (ret <= 0){
        return ENG_DIAG_RET_UNSUPPORT;
    }

    retLen = btwifiimei(buff, len, rsp, rsplen, ret==2?1:0, 0);
    if (retLen > 0 && g_func[WRITE_TO_HOST_DIAG] != NULL){
        g_func[WRITE_TO_HOST_DIAG](rsp, retLen);
    }

    if (ret == 1){ // only write bt wifi mac address
        return ENG_DIAG_NO_RESPONSE;
    }else if (ret == 2){ // also write imei, then pass to cp
        return ENG_DIAG_RET_UNSUPPORT;
    }else {
        return retLen;
    }
}

int bt_wifi_mac_match(char* buff, int len){
    if (buff[7] == 0x5E){
        return 0;
    }else{
        return -1;
    }
}

int phasecheck_handler(char *buff, int len, char *rsp, int rsplen){
    return direct_phschk(buff, len, rsp, rsplen);
}

int phasecheck_match(char* buff, int len){
    if (buff[7] == 0x5F){
        return 0;
    }else{
        return -1;
    }
}

int miscdata_handler(char *buff, int len, char *rsp, int rsplen){
    return eng_diag_product_ctrl(buff, len, rsp, rsplen);
}

int miscdata_offset_handler(char *buff, int len, char *rsp, int rsplen){
    return eng_diag_product_ctrl_ex(buff, len, rsp, rsplen);
}

void register_this_module_ext(struct eng_callback *reg, int *num)
{
    int i = 0;
    int moudles_num = 0;
    ENG_LOG("register_this_module_ext :libmiscdata");

    (reg+moudles_num)->type = 0x0D;
    (reg+moudles_num)->subtype = 0x02;
    (reg+moudles_num)->eng_diag_func = miscdata_handler;
    moudles_num++;

    (reg+moudles_num)->type = 0x0D;
    (reg+moudles_num)->subtype = 0x03;
    (reg+moudles_num)->eng_diag_func = miscdata_handler;
    moudles_num++;

    (reg+moudles_num)->type = 0x0D;
    (reg+moudles_num)->subtype = 0x04;
    (reg+moudles_num)->eng_diag_func = miscdata_handler;
    moudles_num++;

    (reg+moudles_num)->type = 0x0D;
    (reg+moudles_num)->subtype = 0x12;
    (reg+moudles_num)->eng_diag_func = miscdata_offset_handler;
    moudles_num++;

    (reg+moudles_num)->type = 0x0D;
    (reg+moudles_num)->subtype = 0x13;
    (reg+moudles_num)->eng_diag_func = miscdata_offset_handler;
    moudles_num++;

    (reg+moudles_num)->type = 0x0D;
    (reg+moudles_num)->subtype = 0x14;
    (reg+moudles_num)->eng_diag_func = miscdata_offset_handler;
    moudles_num++;

    (reg+moudles_num)->type = 0x5E;
    (reg+moudles_num)->also_need_to_cp = 1;
    (reg+moudles_num)->eng_diag_func = bt_wifi_mac_handler;
    (reg+moudles_num)->eng_cmd_match = bt_wifi_mac_match;
    (reg+moudles_num)->eng_set_writeinterface_func = get_fw_ptr;
    moudles_num++;

    (reg+moudles_num)->type = 0x5F;
    (reg+moudles_num)->eng_diag_func = phasecheck_handler;
    (reg+moudles_num)->eng_cmd_match = phasecheck_match;
    moudles_num++;

    *num = moudles_num;
    ENG_LOG("register_this_module_ext: %d - %d",*num, moudles_num);
}
