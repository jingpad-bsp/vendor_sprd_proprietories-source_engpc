#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <cutils/log.h>
#include <cutils/properties.h>
#include <cutils/sockets.h>

#include "engpcctl.h"
#include "eng_ctl.h"
#include "sprd_fts_log.h"

#define cmd_print(fmt, args...) {\
        SPRD_FTS_LOG(fmt, ##args); \
        printf(fmt, ##args); \
        printf("\n"); \
    }

void printfCmd(LPENGPC_CMD_T lpCmd){
    if (lpCmd != NULL){
        cmd_print("cmd: ");
        cmd_print("    dst = %d ", lpCmd->dst);
        cmd_print("    act = %d ", lpCmd->act);
        if (lpCmd->dst == 0){
            cmd_print("    devName = %s ", lpCmd->dev_s.devName);
            cmd_print("    bootmode = %s ", lpCmd->dev_s.bootmode);
            cmd_print("    portName = %s ", lpCmd->dev_s.portName);
            cmd_print("    portType = %s ", lpCmd->dev_s.portType);
            cmd_print("    portPath = %s ", lpCmd->dev_s.portPath);
            cmd_print("    dataType = %s ", lpCmd->dev_s.dataType);
        }else if(lpCmd->dst == 1){
            cmd_print("    bootmode = %s ", lpCmd->chnl_s.bootmode);
            cmd_print("    name = %s ", lpCmd->chnl_s.name);
            cmd_print("    srcDevName = %s ", lpCmd->chnl_s.srcDevName);
            cmd_print("    srcPortName = %s ", lpCmd->chnl_s.srcPortName);
            cmd_print("    dstDevName = %s ", lpCmd->chnl_s.dstDevName);
            cmd_print("    dstPortName = %s ", lpCmd->chnl_s.dstPortName);
            cmd_print("    act = %s ", lpCmd->chnl_s.act);
            cmd_print("    dataType = %s ", lpCmd->chnl_s.dataType);
            cmd_print("    enable = %s ", lpCmd->chnl_s.enable);
            cmd_print("    ap_process = %s ", lpCmd->chnl_s.ap_process);
        }else{
        }
    }
}

int sendCmd(LPENGPC_CMD_T lpCmd){
    int fd = -1;
    struct sockaddr_in addr;
    int ret = -1;
    int len = 0;
    char rvbuff[2 * 1024 ] = {0};

    if (lpCmd == NULL){
        return -1;
    }

    fd = socket_local_client(SOCK_ENGCTL_NAME, ANDROID_SOCKET_NAMESPACE_ABSTRACT, SOCK_STREAM);
    if (fd < 0) {
        cmd_print("connect %s fail.", SOCK_ENGCTL_NAME);
        return -1;
    }

    if (send(fd, lpCmd, sizeof(ENGPC_CMD_T), 0) > 0) {
        memset(rvbuff, 0, sizeof (rvbuff));
        if ((len = recv(fd, rvbuff, sizeof (rvbuff) - 1, 0)) < 0) {
            cmd_print("recv fail: %s", strerror(errno));
            ret = -2;
        }else{
            cmd_print("recv %d bytes: %s", len, rvbuff);
        }

    }else{
        cmd_print("send fail: %s", strerror(errno));
        ret = -3;
    }

    if (fd >= 0) {
        close(fd);
    }

    return ret;
}

#define STRCPY(d, s)  { \
    if (s != NULL) \
        strncpy(d, s, strlen(s)>(sizeof(d)-1)?(sizeof(d)-1):strlen(s)); \
    }

int engpc_ctl_dev(ENGPC_ACT dAct, char* devname, char* bootmode,
                  char* portName, char* portType, char* portPath, char* dataType){
    ENGPC_CMD_T cmd = {0};
    cmd.dst = ENGPC_DST_DEV;
    cmd.act = dAct;
    STRCPY(cmd.dev_s.devName, devname);
    STRCPY(cmd.dev_s.bootmode, bootmode);
    STRCPY(cmd.dev_s.portName, portName);
    STRCPY(cmd.dev_s.portType, portType);
    STRCPY(cmd.dev_s.portPath, portPath);
    STRCPY(cmd.dev_s.dataType, dataType);

    return sendCmd(&cmd);
}

int engpc_ctl_chnl(ENGPC_ACT cAct, char* bootmode, char* name, char* srcDevName, char* srcPortName,
                  char* dstDevName, char* dstPortName, char* act, char* dataType, char* enable, char* ap_process){
    ENGPC_CMD_T cmd = {0};
    cmd.dst = ENGPC_DST_CHNL;
    cmd.act = cAct;
    STRCPY(cmd.chnl_s.bootmode, bootmode);
    STRCPY(cmd.chnl_s.name, name);
    STRCPY(cmd.chnl_s.srcDevName, srcDevName);
    STRCPY(cmd.chnl_s.srcPortName, srcPortName);
    STRCPY(cmd.chnl_s.dstDevName, dstDevName);
    STRCPY(cmd.chnl_s.dstPortName, dstPortName);
    STRCPY(cmd.chnl_s.act, act);
    STRCPY(cmd.chnl_s.dataType, dataType);
    STRCPY(cmd.chnl_s.enable, enable);
    STRCPY(cmd.chnl_s.ap_process, ap_process);

    return sendCmd(&cmd);
}
