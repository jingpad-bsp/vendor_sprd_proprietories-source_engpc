#ifndef _CHNNEL_DEV_H_
#define _CHNNEL_DEV_H_

#define PORT_NAME_LEN       32
#define PORT_PATH_LEN       64
#define PORT_PROP_LEN       64
#define PORT_RESERVED_LEN   128
#define PORT_DESC_LEN       128
#define BOOTMODE_NAME_LEN   64
#define DEV_NAME_LEN        32
#define MAX_PORT_NUM        64
#define MAX_MODE_NUM        16

#define DEF_BOOT_MODE "normal"

#define DEV_HOST_NAME "pc"
#define DEV_HOST_DIAG_NAME "COM_CP_DIAG"
#define DEV_HOST_WCN_AT_NAME "COM_WCN_AT"

#define DEV_MODEM_NAME "cp"
#define DEV_MODEM_DIAG_NAME "COM_DIAG"
#define DEV_MODEM_AT_NAME "COM_AT"

#define DEV_WCN_NAME "wcn"
#define DEV_WCN_DIAG_NAME "COM_DIAG"

#define DEV_AGDSP_NAME "agdsp"

typedef enum {
    DATA_AT,
    DATA_LOG,
    DATA_DIAG,
    DATA_WCN_AT,
    DATA_WCN_DIAG,
    DATA_AGDSP_LOG,
    DATA_AGDSP_PCM,
    DATA_AGDSP_MEM,
    DATA_AGDSP_MIX,

    DATA_TYPE_UNKNOWN
}DATA_TYPE;

typedef enum {
    PORT_VSER,
    PORT_GSER,
    PORT_SOCK,
    PORT_COM,
    PORT_CHR,
    PORT_INTERFACE,

    PORT_LOOP,
    PORT_UNKNOWN
}PORT_TYPE;

typedef struct ST_DEV_PORT {
    char            portName[PORT_NAME_LEN];
    PORT_TYPE       portType;
    char            portPath[PORT_PATH_LEN];
    DATA_TYPE       dataType;
    char            reserved[PORT_RESERVED_LEN];
    char            desc[PORT_DESC_LEN];
}DEV_PORT, *LPDEV_PORT;

typedef struct ST_BOOTMODE_DEV {
    char        bootmode[BOOTMODE_NAME_LEN];
    int         nPortNum;
    LPDEV_PORT  lpDevPort;
}BOOTMODE_DEV, *LPBOOTMODE_DEV;

typedef struct ST_CHNL_DEV {
    char             devName[DEV_NAME_LEN];
    int              nModeNum;
    LPBOOTMODE_DEV   lpBootDev;
}CHNL_DEV, *LPCHNL_DEV;


#endif