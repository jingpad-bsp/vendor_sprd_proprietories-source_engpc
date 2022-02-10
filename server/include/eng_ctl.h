#ifndef __ENGPC_ENG_CTL_H__
#define __ENGPC_ENG_CTL_H__

#include "dev.h"
#include "channel.h"

#define SOCK_ENGCTL_NAME "engpc_ctl"

#define TYPE_NORMAL_LEN 16

typedef struct{
    char devName[DEV_NAME_LEN];
    char bootmode[BOOTMODE_NAME_LEN];

    char portName[PORT_NAME_LEN];
    char portType[TYPE_NORMAL_LEN];
    char portPath[PORT_PATH_LEN];
    char dataType[TYPE_NORMAL_LEN];
}CTL_DEV_S, *LPCTL_DEV_S;

typedef struct{
    char bootmode[BOOTMODE_NAME_LEN];

    char name[CHNL_NAME_LEN];
    char srcDevName[DEV_NAME_LEN];
    char srcPortName[PORT_NAME_LEN];
    char dstDevName[DEV_NAME_LEN];
    char dstPortName[PORT_NAME_LEN];
    char act[TYPE_NORMAL_LEN];
    char dataType[TYPE_NORMAL_LEN];
    char enable[TYPE_NORMAL_LEN];
    char ap_process[TYPE_NORMAL_LEN];
}CTL_CHNL_S, *LPCTL_CHNL_S;

typedef struct{
    char dst; //0: dev, 1: chnl
    char act; //0: list, 1: add, 2: del, 3: enable, 4:disable
    union{
        CTL_DEV_S   dev_s;
        CTL_CHNL_S  chnl_s;
    };
}ENGPC_CMD_T, *LPENGPC_CMD_T;

#endif