#include <utils/Log.h>
#include <stdlib.h>
#include <stdio.h>
#include<string.h>

#include "sprd_fts_type.h"

extern "C" {
#include "atci.h"
}

int dialPPP(char *req, char *rsp)
{
    const char* response = "\nCONNECT 296\r\n";
    char cmd[128];
    char resp[128];
    char data[1024] = {0};
    int ret = -1;

    memset(data, 0, sizeof(data));
    snprintf(cmd, sizeof(cmd),"START_PPPD");
    ALOGD("dialPPP cmd = %d\n", cmd);

    ret = sendATCmd(0, cmd, data, sizeof(data));

    if(ret == -1) {
        ALOGD("send the command: %s fail\n", cmd);
    } else if (ret == 0) {
        ALOGD("read OK: %s\n", data);
    }

    strncpy(rsp, response, strlen(response)+1);

    return ret;
}

int beforeDialHandle(char *req, char *rsp) {
    const char* response = "\r\nOK\r\n";
    strncpy(rsp, response, strlen(response)+1);

    return 0;
}

int HuangupHandle(char *req, char *rsp) {
    const char* response = "\r\nOK\r\n";
    char cmd[128];
    char data[1024] = {0};
    int ret = -1;

    memset(data, 0, sizeof(data));
    snprintf(cmd, sizeof(cmd),"STOP_PPPD");
    ALOGD("stop PPP cmd = %d\n", cmd);
    ret = sendATCmd(0, cmd, data, sizeof(data));

    if(ret == -1) {
        ALOGD("send the command0: %s fail\n", cmd);
    } else if (ret == 0) {
        ALOGD("read OK 0: %s\n", data);
    }

    strncpy(rsp, response, strlen(response)+1);

    return 0;
}

extern "C" void register_this_module_ext(struct eng_callback *reg, int *num)
{
    int moudles_num = 0;
    ALOGD("register_this_module :dllibtest dialppp");

    //1st command
    sprintf((reg + moudles_num)->at_cmd, "%s", "ATD*99#\r");
    (reg + moudles_num)->eng_linuxcmd_func = dialPPP;
    moudles_num++;

    //2nd command
    sprintf((reg + moudles_num)->at_cmd, "%s", "AT\r");
    (reg + moudles_num)->eng_linuxcmd_func = beforeDialHandle;
    moudles_num++;

    //3rd command
    sprintf((reg + moudles_num)->at_cmd, "%s", "ATE0V1\r");
    (reg + moudles_num)->eng_linuxcmd_func = beforeDialHandle;
    moudles_num++;

    //4th command
    sprintf((reg + moudles_num)->at_cmd, "%s", "ATH E1\r");
    (reg + moudles_num)->eng_linuxcmd_func = HuangupHandle;
    moudles_num++;

    //5th command
    sprintf((reg + moudles_num)->at_cmd, "%s", "ATD#777\r");
    (reg + moudles_num)->eng_linuxcmd_func = dialPPP;
    moudles_num++;

    *num = moudles_num;
    ALOGD("register_this_module_ext: %d - %d",*num, moudles_num);
}
