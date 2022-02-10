#ifndef __ENG_MODULES_H__
#define __ENG_MODULES_H__

#include "sprd_fts_list.h"
#include "sprd_fts_type.h"

typedef enum {
    DYMIC_RET_NO_DEAL = 0,
    DYMIC_RET_DEAL_SUCCESS = 1,
    DYMIC_RET_ALSO_NEED_TO_CP = 2,
    DYMIC_RET_NO_RESPONSE_TO_PC = 3,
    DYMIC_RET_ENCODE_TO_DEST = 4,
} DYMIC_RET_E;

typedef struct eng_modules_info
{
    struct  list_head node;
    struct  eng_callback callback;
}eng_modules;


#endif