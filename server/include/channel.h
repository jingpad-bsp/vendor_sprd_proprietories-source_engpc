#ifndef __CHANNEL_H_
#define __CHANNEL_H_

#include "dev.h"

#define CHNL_NAME_LEN   128
#define CHNL_DESC_LEN   128
#define CHNL_RESVED_LEN   128

#define MAX_CHNL_NUM    64

typedef enum
{
    WORK_MODE_F, //src->dst && dst->src
    WORK_MODE_S, //src->dst

    WORK_MODE_UNKNOW,
}WORK_MODE;

typedef struct ST_CHNL_PROP
{
    char        name[CHNL_NAME_LEN];
    char        srcDevName[DEV_NAME_LEN];
    char        srcPortName[PORT_NAME_LEN];
    char        dstDevName[DEV_NAME_LEN];
    char        dstPortName[PORT_NAME_LEN];
    int         act;
    DATA_TYPE   dataType;
    int         enable;
    int         ap_process;
    char        reserved[CHNL_RESVED_LEN];
    char        desc[CHNL_DESC_LEN];
}CHNL_PROP, *LPCHNL_PROP;

#endif