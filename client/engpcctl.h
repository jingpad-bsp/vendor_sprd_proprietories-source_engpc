#ifndef __ENG_CTL_H__
#define __ENG_CTL_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef enum{
    ENGPC_DST_DEV,
    ENGPC_DST_CHNL,

    ENGPC_DST_NONE,
}ENGPC_DST;

typedef enum{
    ENGPC_ACT_LIST,
    ENGPC_ACT_ADD,
    ENGPC_ACT_DEL,
    ENGPC_ACT_ENABLE,
    ENGPC_ACT_DISABLE,

    ENGPC_ACT_NONE,
}ENGPC_ACT;

/*
portType:   1: vser
            2: gser
            3: sock
            4: com
            5: chr
            6: interface
            7: loop

dataType:   1: DATA_AT,
            2: DATA_LOG,
            3: DATA_DIAG,
            4: DATA_WCN_AT,
            5: DATA_WCN_DIAG,
            6: DATA_AGDSP_LOG,
            7: DATA_AGDSP_PCM,
            8: DATA_AGDSP_MEM,
            9: DATA_AGDSP_MIX,
*/
int engpc_ctl_dev(ENGPC_ACT dAct, char* devname, char* bootmode,
                  char* portName, char* portType, char* portPath, char* dataType);

/*
act: 
        F: means src->dst && dst->src
        S: means src->dst

dataType:   1: DATA_AT,
            2: DATA_LOG,
            3: DATA_DIAG,
            4: DATA_WCN_AT,
            5: DATA_WCN_DIAG,
            6: DATA_AGDSP_LOG,
            7: DATA_AGDSP_PCM,
            8: DATA_AGDSP_MEM,
            9: DATA_AGDSP_MIX,

enable: 
        0: means this chnl don't work
        1: means this chnl work

ap_process: 
        0: means no need so process
        1: means need so process

*/
int engpc_ctl_chnl(ENGPC_ACT cAct, char* bootmode, char* name, char* srdDevName, char* srcPortName,
                  char* dstDevName, char* dstPortName, char* act, char* dataType, char* enable, char* ap_process);

#ifdef __cplusplus
}
#endif

#endif
