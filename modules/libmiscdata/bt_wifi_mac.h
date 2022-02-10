#ifndef _BT_WIFI_MAC_H
#define _BT_WIFI_MAC_H

#define RW_MASK 0x80 
#define WRITE_MODE 0
#define RM_VALID_CMD_MASK 0x7F

#define MAX_IMEI_LENGTH 8
#define MAX_IMEI_STR_LENGTH 15
#define MAX_BTADDR_LENGTH 6
#define MAX_WIFIADDR_LENGTH 6
#define GPS_NVINFO_LENGTH 44
#define DIAG_HEADER_LENGTH 8

#define DIAG_CMD_IMEI1BIT 0x01
#define DIAG_CMD_IMEI2BIT 0x02
#define DIAG_CMD_IMEI3BIT 0x10
#define DIAG_CMD_IMEI4BIT 0x20
#define DIAG_CMD_BTBIT 0x04
#define DIAG_CMD_WIFIBIT 0x40

#define DIAG_CMD_READ 0x80 

#define IMEI_NUM 4

#define ENG_DEV_PATH_LEN 260

#define ENG_IMEI1_CONFIG_FILE "/mnt/vendor/imei1.txt"
#define ENG_IMEI2_CONFIG_FILE "/mnt/vendor/imei2.txt"
#define ENG_IMEI3_CONFIG_FILE "/mnt/vendor/imei3.txt"
#define ENG_IMEI4_CONFIG_FILE "/mnt/vendor/imei4.txt"

typedef struct {
    unsigned char imei1[MAX_IMEI_LENGTH];
    unsigned char imei2[MAX_IMEI_LENGTH];
    unsigned char btaddr[MAX_BTADDR_LENGTH];
    unsigned char gpsinfo[GPS_NVINFO_LENGTH];
    unsigned char wifiaddr[MAX_WIFIADDR_LENGTH];
    unsigned char reserved1[2];
    unsigned char imei3[MAX_IMEI_LENGTH];
    unsigned char imei4[MAX_IMEI_LENGTH];
    unsigned char reserved2[16];
} REF_NVWriteDirect_T;

typedef enum
{
    ENG_BT_MAC,
    ENG_WIFI_MAC
} MacType;

void eng_diag_char2hex(unsigned char *hexdata, char *chardata);
int eng_diag_read_imei(REF_NVWriteDirect_T *direct, int num);
int eng_diag_write_imei(REF_NVWriteDirect_T *direct, int num);
char MAKE1BYTE2BYTES(unsigned char high4bit, unsigned char low4bit);
void ImeiConvNV2Str(unsigned char *nvImei, unsigned char *szImei);
void ImeiConvStr2NV(unsigned char *szImei, unsigned char *nvImei);

int eng_btwifimac_write(char* bt_mac, char* wifi_mac);
int eng_btwifimac_read(char* mac, MacType type);
int btwifi_checkCRC(char* buff);
int btwifi_checkValid(char *buff);
int btwifiimei(char *buf, int len, char *rsp, int rsplen, int cp_ap_proc, int ap_cali);

#endif