#ifndef __SPRD_FTS_CB_H__
#define __SPRD_FTS_CB_H__

#include <linux/types.h>
#include "sprd_fts_list.h"

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

struct eng_callback_func{
    char moudel_des[128];
    void(*eng_cb)(struct fw_callback *reg);
};

typedef int (*QUERYINTERFACE)(char *module_desc,const void** ptrFunc);

struct fw_callback{
    QUERYINTERFACE ptfQueryInterface;
};

typedef struct eng_callbacks_info
{
    struct  list_head node;
    struct  eng_callback_func callback;
}eng_callbacks;

struct eng_fw_des_ptrFunc{
    char func_des[128];
    int  ptrFunc;
};

typedef struct eng_fw_ptrFunc_info
{
    struct list_head node;
    struct eng_fw_des_ptrFunc fw_ptrFunc;
}eng_fw_ptrFunc;


#ifdef __cplusplus
}
#endif

#endif
