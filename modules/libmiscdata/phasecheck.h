#ifndef _ENG_PHASE_CHECK_H
#define _ENG_PHASE_CHECK_H

typedef enum {
  IMEI_ERR_NONE = 0,
  IMEI_CRC_ERR,
  IMEI_CMD_ERR,
  IMEI_SAVE_ERR,
  IMEI_READ_ERR
} ERR_IMEI_E;

#define NUM_ELEMS(x) (sizeof(x) / sizeof(x[0]))
#define NVITEM_ERROR_E int
#define NVERR_NONE 0

#define RW_MASK 0x80  //(BIT_7)
#define WRITE_MODE 0
#define RM_VALID_CMD_MASK 0x7f
#define DIAG_CMD_GPS_AUTO_TEST 0x3A
#define MSG_NACK 0
#define MSG_ACK 1

#define HASH_LEN 40

#define MAX_SN_LEN 24
#define MAX_STATION_NUM 15
#define MAX_STATION_NAME_LEN 10
#define MAX_LAST_DESCRIPTION_LEN 32
#define SP09_SPPH_MAGIC_NUMBER   0x53503039

#define SP15_MAX_SN_LEN                  64
#define SP15_MAX_STATION_NUM             20
#define SP15_MAX_STATION_NAME_LEN        15
#define SP15_SPPH_MAGIC_NUMBER   0x53503135

typedef struct _SP09_PHASE_CHECK_HEADER
{
    unsigned int Magic; //"SP09"
    unsigned char SN[MAX_SN_LEN];   //SN,SN_LEN=24
    unsigned char SN2[MAX_SN_LEN];  //Add for Mobile

    unsigned int StationNum;   //The test station number of the testing
    unsigned char StationName[MAX_STATION_NUM][MAX_STATION_NAME_LEN];

    unsigned char Reserved[13]; //value: 0
    unsigned char SignFlag; // internal flag
    char szLastFailDescription[MAX_LAST_DESCRIPTION_LEN];
    unsigned short iTestSign; // Bit0~Bit14 --> station0 ~ station14 if tested. 0:tested,1:not tested.
    unsigned short iItem; // Part1:Bit0~Bit14 indicate test station,0:pass,1:fail
    // Part2:Bit15 set to 0;
}SP09_TEST_TRACK_HEADER_T;

typedef struct _SP09_PHASE_CHECK_S
{
    SP09_TEST_TRACK_HEADER_T header;
}SP09_TEST_DATA_INFO_T;

/**/
typedef struct _SP15_PHASE_CHECK_HEADER
{
    unsigned int Magic; //"SP15"
    unsigned char SN[SP15_MAX_SN_LEN];   //SN,SN_LEN=24
    unsigned char SN2[SP15_MAX_SN_LEN];  //Add for Mobile

    unsigned int StationNum;   //The test station number of the testing
    unsigned char StationName[SP15_MAX_STATION_NUM][SP15_MAX_STATION_NAME_LEN];

    unsigned char Reserved[13]; //value: 0
    unsigned char SignFlag; // internal flag
    char szLastFailDescription[MAX_LAST_DESCRIPTION_LEN];
    unsigned long iTestSign; // Bit0~Bit14 --> station0 ~ station14 if tested. 0:tested,1:not tested.
    unsigned long iItem; // Part1:Bit0~Bit14 indicate test station,0:pass,1:fail
    // Part2:Bit15 set to 0;
}SP15_TEST_TRACK_HEADER_T;

typedef struct _SP15_PHASE_CHECK_S
{
    SP15_TEST_TRACK_HEADER_T header;
}SP15_TEST_DATA_INFO_T;

int direct_phschk(char *buf, int len, char *rsp, int rsplen);

#endif