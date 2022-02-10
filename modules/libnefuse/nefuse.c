#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include <getopt.h>
#include <ctype.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/types.h>

#include "sprd_fts_type.h"
#include "sprd_fts_diag.h"
#include "sprd_fts_log.h"

//get pmic efuse define AT+SPPMICTEST=GET_EFUSE,BLOCK ID
#define AT_PMICEFUSE "AT+SPPMICTEST="
//get ap efuse define AT+SPBBTEST=GET_EFUSE,BLOCK ID
#define AT_APEFUSE "AT+SPBBTEST="
#define EFUSE_PARA "GET_EFUSE"
#define RSP_PMICEFUSE "+SPPMICTEST:EFUSE="
#define RSP_APEFUSE "+SPBBTEST:EFUSE="

#define AP_EFUSE "/sys/bus/nvmem/devices/sprd-cache-efuse0/nvmem"
//#define AP_EFUSE "/sys/bus/nvmem/devices/sprd-efuse0/nvmem"
#define PMIC_EFUSE "/sys/bus/nvmem/devices/sc27xx-efuse0/nvmem"

#define AP_BLOCK_SIZE (4)
#define PMIC_BLOCK_SIZE (2)

static char pmic_efuse_cmd_param[64] = {0};
static char ap_efuse_cmd_param[64] = {0};
static char ap_efuse_blk_param[8] = {0};
static char pmic_efuse_blk_param[8] = {0};

static int pmic_efuse_read(unsigned int blk, char *data) {
  char buf[5] = {0};
  off_t curpos, offset;
  int fd = -1, ret = 0;
  if (data == 0) return -1;

  ENG_LOG("%s efuse read index = %d", __FUNCTION__, blk);
  fd = open(PMIC_EFUSE, O_RDONLY);
  if (fd < 0) {
    ALOGE("%s()->Line:%d; %s open error, fd = %d. \n", __FUNCTION__, __LINE__,
          PMIC_EFUSE, fd);
    return -2;
  }
  offset = blk * PMIC_BLOCK_SIZE;
  curpos = lseek(fd, offset, SEEK_CUR);
  if (curpos == -1) {
    ALOGE("%s()->Line:%d; lseek error\n", __FUNCTION__, __LINE__);
    close(fd);
    return -3;
  }
  ret = read(fd, buf, 2);
  if (ret <= 0) {
    ALOGE("%s()->Line:%d; read efuse block(%d) data error, retcode = %d; \n",
          __FUNCTION__, __LINE__, blk, ret);
    close(fd);
    return -4;
  }
  close(fd);
  sprintf(data, "%02x%02x", buf[1], buf[0]);
  ALOGD("buf=%s\n", data);
  return 1;
}

static int ap_efuse_read(unsigned int blk, char *data) {
  char buf[5] = {0};
  off_t curpos, offset;
  int fd = -1, ret = 0;
  if (data == 0) return -1;

  ENG_LOG("%s efuse read index = %d", __FUNCTION__, blk);
  fd = open(AP_EFUSE, O_RDONLY);
  if (fd < 0) {
    ALOGE("%s()->Line:%d; %s open error, fd = %d. \n", __FUNCTION__, __LINE__,
          AP_EFUSE, fd);
    return -2;
  }
  offset = blk * AP_BLOCK_SIZE;
  curpos = lseek(fd, offset, SEEK_CUR);
  if (curpos == -1) {
    ALOGE("%s()->Line:%d; lseek error\n", __FUNCTION__, __LINE__);
    close(fd);
    return -3;
  }
  ret = read(fd, buf, 4);
  if (ret <= 0) {
    ALOGE("%s()->Line:%d; read efuse block(%d) data error, retcode = %d; \n",
          __FUNCTION__, __LINE__, blk, ret);
    close(fd);
    return -4;
  }
  close(fd);
  sprintf(data, "%02x%02x%02x%02x", buf[3], buf[2], buf[1], buf[0]);
  ALOGD("buf=%s\n", data);
  return 1;
}

static int pmic_efuse_handle(char *buf, char *rsp)
{
    char value[10] = {0};
    char *ptr = NULL;
    unsigned int nlen = 0;
    int index, ret;

    if (NULL == buf)
    {
        ALOGE("%s,null pointer", __FUNCTION__);
        sprintf(rsp, "\r\nERROR\r\n");
        return rsp != NULL ? strlen(rsp) : 0;
    }

    if(buf[0] == 0x7e)
    {
        ptr = buf + 1 + sizeof(MSG_HEAD_T);
    }
    else
    {
        ptr = strdup(buf);
    }
    ENG_LOG("%s pmic efuse ptr = %s", __FUNCTION__, ptr);

    if (strncasecmp(ptr, AT_PMICEFUSE,strlen(AT_PMICEFUSE)) == 0){
        memset(pmic_efuse_cmd_param, 0, sizeof(pmic_efuse_cmd_param));
        nlen = strlen(buf)-strlen(AT_PMICEFUSE);
        nlen = (nlen >= sizeof(pmic_efuse_cmd_param)-1)?(sizeof(pmic_efuse_cmd_param)-1):nlen;
        strncpy(pmic_efuse_cmd_param, ptr+strlen(AT_PMICEFUSE), strlen(EFUSE_PARA));
        ENG_LOG("%s pmic efuse param  = %s", __FUNCTION__, pmic_efuse_cmd_param);
	strncpy(pmic_efuse_blk_param, ptr+strlen(AT_PMICEFUSE) + strlen(EFUSE_PARA) + 1, 2);
        ENG_LOG("%s pmic efuse  blk param  = %s", __FUNCTION__, pmic_efuse_blk_param);
	index = atoi(pmic_efuse_blk_param);
	ret = pmic_efuse_read(index, value);
	if(ret == 1) {
		sprintf(rsp, "%s0x%s\r\nOK\r\n", RSP_PMICEFUSE, value);
		ENG_LOG("%s pmic efuse index = %d, rsp: %s", __FUNCTION__, index, rsp);
	}else{
		sprintf(rsp, "\r\n+CME ERROR:%d\r\n", ret);
	}
    }else{
        sprintf(rsp, "\r\n+CME ERROR:4\r\n");
    }

    return strlen(rsp);
}

static int ap_efuse_handle(char *buf, char *rsp)
{
    char value[10] = {0};
    char *ptr = NULL;
    unsigned int nlen = 0;
    int index, ret;

    if (NULL == buf)
    {
        ALOGE("%s,null pointer", __FUNCTION__);
        sprintf(rsp, "\r\nERROR\r\n");
        return rsp != NULL ? strlen(rsp) : 0;
    }

    if(buf[0] == 0x7e)
    {
        ptr = buf + 1 + sizeof(MSG_HEAD_T);
    }
    else
    {
        ptr = strdup(buf);
    }
    ENG_LOG("%s ap efuse ptr = %s", __FUNCTION__, ptr);

    if (strncasecmp(ptr, AT_APEFUSE,strlen(AT_APEFUSE)) == 0){
        memset(ap_efuse_cmd_param, 0, sizeof(ap_efuse_cmd_param));
        nlen = strlen(buf)-strlen(AT_APEFUSE);
        nlen = (nlen >= sizeof(ap_efuse_cmd_param)-1)?(sizeof(ap_efuse_cmd_param)-1):nlen;
        strncpy(ap_efuse_cmd_param, ptr+strlen(AT_APEFUSE), strlen(EFUSE_PARA));
        ENG_LOG("%s ap efuse param  = %s", __FUNCTION__, ap_efuse_cmd_param);
	strncpy(ap_efuse_blk_param, ptr+strlen(AT_APEFUSE) + strlen(EFUSE_PARA) + 1, 2);
        ENG_LOG("%s ap efuse  blk param  = %s", __FUNCTION__, ap_efuse_blk_param);
	index = atoi(ap_efuse_blk_param);
	ret = ap_efuse_read(index, value);
	if(ret == 1){
		sprintf(rsp, "%s0x%s\r\nOK\r\n", RSP_APEFUSE, value);
		ENG_LOG("%s ap efuse index = %d, rsp: %s", __FUNCTION__, index, rsp);
	}else{
		sprintf(rsp, "\r\n+CME ERROR:%d\r\n", ret);
	}
    }else{
	sprintf(rsp, "\r\n+CME ERROR:4\r\n");
    }

    return strlen(rsp);
}

void register_this_module_ext(struct eng_callback *reg, int *num)
{
    int moudles_num = 0;
    ENG_LOG("register_this_module_ext :libdloader");

    sprintf((reg + moudles_num)->at_cmd, "%s", AT_PMICEFUSE);
    (reg + moudles_num)->eng_linuxcmd_func = pmic_efuse_handle;
    moudles_num++;

    sprintf((reg + moudles_num)->at_cmd, "%s", AT_APEFUSE);
    (reg + moudles_num)->eng_linuxcmd_func = ap_efuse_handle;
    moudles_num++;

    *num = moudles_num;
    ENG_LOG("register_this_module_ext: %d - %d",*num, moudles_num);
}
