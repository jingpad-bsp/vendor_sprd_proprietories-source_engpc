#ifndef __SPRD_FTS_DIAG_H__
#define __SPRD_FTS_DIAG_H__

#include <linux/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
***    DIAG_HEADER DESCRIPTION   ****
*/

typedef struct msg_head_tag {
  unsigned int seq_num;  // Message sequence number, used for flow control
  unsigned short len;    // The totoal size of the packet "sizeof(MSG_HEAD_T)
  unsigned char type;     // Main command type
  unsigned char subtype;  // Sub command type
} __attribute__((packed)) MSG_HEAD_T;

#define DIAG_HEADER_LENGTH (sizeof(MSG_HEAD_T))

typedef struct {
  unsigned short cmd;     // DIAG_AP_CMD_E
  unsigned short length;  // Length of structure
} TOOLS_DIAG_AP_CMD_T;

typedef struct {
  unsigned int operate;  // 0: Get ADC   1: Load ADC    2: Save ADC
  unsigned int parameters[12];
} TOOLS_AP_ADC_REQ_T;

typedef struct {
  unsigned short status;  // ==0: success, != 0: fail
  unsigned short length;  // length of  result
} TOOLS_DIAG_AP_CNF_T;

typedef enum {
  DIAG_AP_CMD_ADC = 0x0001,
  DIAG_AP_CMD_LOOP,
  DIAG_AP_CMD_FILE_OPER,
  DIAG_AP_CMD_SWITCH_CP,
  DIAG_AP_CMD_BKLIGHT = 0x0005,
  DIAG_AP_CMD_PWMODE = 0x0007,
  DIAG_AP_CMD_CHANGE = 0x0010,
  DIAG_AP_CMD_READ_CURRENT = 0x0011,
  DIAG_AP_CMD_TSX_DATA = 0x0019,
  DIAG_AP_CMD_TSX_DATA_EXT = 0x0023,//include osc temprature x 4
  DIAG_AP_CMD_GET_MODEM_MODE = 0x0012,
  DIAG_AP_CMD_MODEM_DB_ATTR = 0x000e,
  DIAG_AP_CMD_MODEM_DB_READ = 0x000f,
  DIAG_AP_CMD_READ_MMI = 0x0013, //Read MMI
  DIAG_AP_CMD_WRITE_MMI = 0x0014, //Write MMI
  DIAG_AP_CMD_TEE_PRODUCTION = 0x001d,
  DIAG_AP_CMD_SET_MAX_CURRENT = 0x0022,  //Set Max Charge Current
  DIAG_AP_CMD_TSX_RAW_DATA = 0x0024, //tsx raw data
  DIAG_AP_CMD_SET_PMIC_AFC_CDAC   = 0x0025,
  DIAG_AP_CMD_PMIC_AFC_AMP_CALI   = 0x0026,
  DIAG_AP_CMD_LOAD_PMIC_AFC_DATA  = 0x0027,
  DIAG_AP_CMD_SAVE_PMIC_AFC_DATA  = 0x0028,
  DIAG_AP_CMD_SET_PMIC_AFC_MODE   = 0x0029,

  MAX_DIAG_AP_CMD
} DIAG_AP_CMD_E;

#ifdef __cplusplus
}
#endif

#endif
