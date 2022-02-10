#ifndef TSX_RAWDATA_H_
#define TSX_RAWDATA_H_

typedef unsigned short uint16;
typedef unsigned int uint32;

typedef struct 
{
    uint16  cmd;        // DIAG_AP_CMD_E
    uint16  length;     // Length of structure
    uint32  subcmd;     // 0 = Save, 1 =Load 
    uint32  status;     // 0 = success
    uint32  datalen;    // Data length
} DIAG_TSX_DATA_HEAD;

typedef struct
{
  unsigned int freq;
  unsigned int temprature;
}TSX_DATA_T;

typedef struct
{
  unsigned int cmd;
  unsigned int res_status;
  TSX_DATA_T value[2];
}TOOLS_DIAG_AP_TSX_DATA_T;

typedef struct
{
  unsigned int freq;
  unsigned int temprature;
  unsigned int osctemprature;
}TSX_DATA_EXT_T;

typedef struct
{
  unsigned int cmd;
  unsigned int res_status;
  TSX_DATA_EXT_T value[4];
}TOOLS_DIAG_AP_TSX_DATA_EXT_T;

#endif