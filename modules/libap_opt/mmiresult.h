#ifndef __MMIRESULT_H__
#define __MMIRESULT_H__

#include "sprd_fts_type.h"
#include "sprd_fts_diag.h"
#include "sprd_fts_log.h"

#define MAX_BBAT_ITEM_COUNT 64
#define MAX_LINE_LEN 256

#define BBAT_TEST_FILE_PATH "/mnt/vendor/BBATtest.txt"
#define PCBA_TEST_FILE_PATH "/mnt/vendor/PCBAtest.txt"
#define WHOLE_PHONE_TEST_FILE_PATH "/mnt/vendor/wholephonetest.txt"
#define ENG_PCBA_SUPPORT_CONFIG "/vendor/etc/PCBA.conf"
#define ENG_BBAT_SUPPORT_CONFIG "/vendor/etc/BBAT.conf"

#define DIAG_CMD_APCALI_TYPE 0x62
#define DIAG_CMD_APCALI_SUBTYPE 0x00

typedef struct {
  char type_id;
  char function_id;
  char support;
  char status;
}TEST_NEW_RESULT_INFO;

typedef struct hardware_result{
  char id;
  char support;
}hardware_result;

typedef struct
{
  unsigned short  uType; //00: whole phone test; 01: PCBA test ; 02: BBAT test
  char uBuff[256]; //16*16
}TOOLS_DIAG_MMI_CIT_T;

int eng_diag_read_mmi(char *buf, int len, char *rsp, int rsplen);
int eng_diag_write_mmi(char *buf, int len, char *rsp, int rsplen);
int eng_init_test_file(void);

hardware_result bbat_support_result[64];

#endif