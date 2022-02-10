#ifndef __SPRD_FTS_TYPE_H__
#define __SPRD_FTS_TYPE_H__

#include <linux/ioctl.h>
#include <linux/types.h>
#include "sprd_fts_diag.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef byte
typedef unsigned char  byte;
#endif

#ifndef uchar 
typedef unsigned char  uchar;
#endif

#ifndef uint
typedef unsigned int   uint;
#endif // uint

#ifndef ushort
typedef unsigned short ushort;
#endif

typedef int (*DYMIC_WRITETOPC_FUNC)(char *rsp, int len);

struct eng_callback{
    unsigned int diag_ap_cmd;   //data area: unsigned int for data command
    unsigned char type;         //command
    unsigned char subtype;      //data command
    char at_cmd[32];            //at comd
    int also_need_to_cp;
    int (*eng_diag_func)(char *buf, int len, char *rsp, int rsplen);
    int (*eng_linuxcmd_func)(char *req, char *rsp);
    int (*eng_set_writeinterface_func)(DYMIC_WRITETOPC_FUNC * write_interface_ptr);
    int (*eng_cmd_match)(char* buf, int len);
};

enum
{
    ENG_DIAG_NO_RESPONSE = -255,
    ENG_DIAG_RET_UNSUPPORT = -256,
    ENG_DIAG_RET_ERR = 0,
};

typedef enum {
  WRITE_TO_START = 0,
  WRITE_TO_HOST_DIAG = WRITE_TO_START,
  WRITE_TO_HOST_LOG,
  WRITE_TO_HOST_AT,
  WRITE_TO_MODEM_DIAG,
  WRITE_TO_MODEM_LOG,
  WRITE_TO_MODEM_AT,
  WRITE_TO_NPISO_DIAG,
  WRITE_TO_NPISO_LOG,
  WRITE_TO_NPISO_AT,
  WRITE_TO_MAX = WRITE_TO_START + 16,
} WRITE_INTERFACE_E;

#ifdef __cplusplus
}
#endif

#endif
