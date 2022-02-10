#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include <getopt.h>
#include <ctype.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/reboot.h>
#include <unistd.h>
#include <cutils/android_reboot.h>
#include <cutils/properties.h>

#include "sprd_fts_type.h"
#include "sprd_fts_log.h"

#define SPRD_VERS "ro.build.description"

#define AT_GETPROP "AT+PROP="

#define AT_EMMCDDRSIZE "AT+EMMCDDRSIZE"
#define AT_RSP "+EMMCDDRSIZE: "

#define EMMC_SIZE_PATH   "/sys/block/mmcblk0/size"
#define UFS_SIZE_PATH    "/sys/block/sda/size"
#define DDR_SIZE_PATH    "/proc/sprd_dmc/property"

enum{
    SIZE_DDR,
    SIZE_EMMC,

    SIZE_NONE
};

long getSize(int type){
    int fd;
    char buffer[64]={0};
    char *endptr = NULL;
    char *path = NULL;

    if (type == SIZE_DDR){
        path = DDR_SIZE_PATH;
    }else if(type == SIZE_EMMC){
        if (0 == access(EMMC_SIZE_PATH,F_OK))
            path = EMMC_SIZE_PATH;
        else
            path = UFS_SIZE_PATH;
    }else{
        return 0;
    }

    fd = open(path,O_RDONLY);
    if(fd < 0){
        ENG_LOG("open %s fail\n", path);
        return 0;
    }
    if(read(fd,buffer,sizeof(buffer)) <= 0){
        ENG_LOG("read %s fail\n", path);
        return 0;
    }
    ENG_LOG("read %s succ: size = %s", path, buffer);
    close(fd);
    return strtoul(buffer,&endptr,0);;
}

static int getEmmcDDRSize_handle(char *buff, char *rsp)
{
    char *ptr = NULL;
    char cmd_buf[256] = {0};
    int ret = -1;
    if (NULL == buff)
    {
        ENG_LOG("%s,null pointer", __FUNCTION__);
        if(NULL == rsp){
            return 0;
        }
        sprintf(rsp, "ERROR\r\n");
        return strlen(rsp);
    }

    if(buff[0] == 0x7e)
    {
        ptr = buff + 1 + sizeof(MSG_HEAD_T);
    }
    else
    {
        ptr = strdup(buff);
    }

    ENG_LOG("%s ptr = %s", __FUNCTION__, ptr);
    if (strncasecmp(ptr, AT_EMMCDDRSIZE,strlen(AT_EMMCDDRSIZE)) == 0){
        double szEmmc = 0;
        long szDDR = 0;
        char buff[32] = {0};

        szEmmc = (double)getSize(SIZE_EMMC);
        memset(buff, 0, sizeof(buff));
        sprintf(buff, "%4.2f GB", ceil(szEmmc/2/1024/1024));
        strcat(rsp, buff);

        strcat(rsp, "+");

        szDDR = getSize(SIZE_DDR);
        memset(buff, 0, sizeof(buff));
        if (szDDR >= 1024){
            sprintf(buff, "%4.2f GB", ((float)szDDR)/1024);
        }else{
            sprintf(buff, "%4.2f MB", ((float)szDDR));
        }
        strcat(rsp, buff);
        strcat(rsp, "\r\n");

    }else{
        sprintf(rsp, "ERROR\r\n");
    }

    if(buff[0] != 0x7e){
        free(ptr);
    }

    return strlen(rsp);
}

static int ap_version_handler(char *buf, int len, char *rsp, int rsplen){
    char sprdver[256] = {0};
    MSG_HEAD_T *msg_head_ptr;
    int rlen = 0;

    memcpy(rsp, buf, sizeof(MSG_HEAD_T)+1);
    msg_head_ptr = (MSG_HEAD_T *)(rsp + 1);
    msg_head_ptr->len = sizeof(MSG_HEAD_T);

    memset(sprdver, 0, sizeof(sprdver));
    property_get(SPRD_VERS, sprdver, "UNKNOWN VERSION");
    ENG_LOG("%s: %s", __FUNCTION__, sprdver);

    rlen = strlen(sprdver)+1;
    msg_head_ptr->len+=rlen;

    char* data = (char*)msg_head_ptr + sizeof(MSG_HEAD_T);
    sprintf(data, "%s", sprdver);
    data[rlen] = 0x7e;

    ENG_LOG("%s:rlen=%d; %s", __FUNCTION__, rlen, rsp);
    return msg_head_ptr->len+2;
}

#define ENG_STREND "\r\n"

static int getProp_handle(char *buff, char *rsp){
    char ptr_cmd[1];
    char *ptr_key, *ptr_val, *req;
    char buf[PROPERTY_VALUE_MAX] = {0};

    char *ptr = NULL;
    char cmd_buf[256] = {0};
    int ret = -1;

    if (NULL == buff)
    {
        ENG_LOG("%s,null pointer", __FUNCTION__);
        if(NULL == rsp){
          return 0;
        }
        return sprintf(rsp, "ERROR\r\n");
    }

    if(buff[0] == 0x7e)
    {
        ptr = buff + 1 + sizeof(MSG_HEAD_T);
    }
    else
    {
        ptr = strdup(buff);
    }

    req = strchr(ptr, '=');
    req++;
    ptr_cmd[0] = *req;
    req = strchr(req, ',');
    req++;
    if (*req != '[') {
        if(buff[0] != 0x7e){   //AT is correct that not free,and will be used later
            free(ptr);
        }
    return -1;
    }
    req++;
    ptr_key = req;
    req = strchr(req, ']');
    *req = '\0';

    if (ptr_cmd[0] == '0') {
        property_get(ptr_key, buf, "NOT FOUND");
        if (!strncmp(buf, "NOT FOUND", sizeof("NOT FOUND") - 1)) {
            sprintf(rsp, "%s%s%s", ENG_STREND, buf, ENG_STREND);
        } else {
            sprintf(rsp, "%s%s%s%s%s", ENG_STREND, "[", buf, "]", ENG_STREND);
        }
    } else if (ptr_cmd[0] == '1') {
        req++;
        if (*req != '[') {
            sprintf(rsp, "%s%s%s", ENG_STREND, "ERROR", ENG_STREND);
        }
        req++;
        ptr_val = req;
        req = strchr(req, ']');
        *req = '\0';
        property_set(ptr_key, ptr_val);
        sprintf(rsp, "%s%s%s%s%s%s%s", ENG_STREND, "[", ptr_key, "][", ptr_val, "]", ENG_STREND);
    }

    if(buff[0] != 0x7e){  //AT is correct that not free,and will be used later
        free(ptr);
    }
    return 0;
}

int eng_diag_bootreset(unsigned char *buf, int len, char *rsp)
{
    MSG_HEAD_T *msg_head_ptr;
    int rlen = 0;
    char *data="OK";

    memcpy(rsp, buf, sizeof(MSG_HEAD_T)+1);
    msg_head_ptr = (MSG_HEAD_T *)(rsp + 1);
    rlen = strlen(data)+1;
    msg_head_ptr->len = sizeof(MSG_HEAD_T)+rlen;

    char *temp= (char*)msg_head_ptr + sizeof(MSG_HEAD_T);
    sprintf(temp, "%s", data);
    temp[rlen] = 0x7e;
    ENG_LOG("%s:rlen=%d; %s", __FUNCTION__, rlen, rsp);

    sync();
    android_reboot(ANDROID_RB_RESTART2, 0, "cftreboot");
    return msg_head_ptr->len+2;
  }


void register_this_module_ext(struct eng_callback *reg, int *num)
{
    int moudles_num = 0;
    ENG_LOG("register_this_module_ext :libapcomm");

    sprintf((reg + moudles_num)->at_cmd, "%s", AT_EMMCDDRSIZE);
    (reg + moudles_num)->eng_linuxcmd_func = getEmmcDDRSize_handle;
    moudles_num++;

    sprintf((reg + moudles_num)->at_cmd, "%s", AT_GETPROP);
    (reg + moudles_num)->eng_linuxcmd_func = getProp_handle;
    moudles_num++;

    (reg+moudles_num)->type = 0x00; //main cmd
    (reg+moudles_num)->subtype = 0x02; //sub cmd
    (reg+moudles_num)->eng_diag_func = ap_version_handler; // rsp function ptr
    moudles_num++;

    (reg+moudles_num)->type = 0x0c; //main cmd
    (reg+moudles_num)->subtype = 0x08; //sub cmd
    (reg+moudles_num)->eng_diag_func = eng_diag_bootreset; // rsp function ptr
    moudles_num++;

    *num = moudles_num;
    ENG_LOG("register_this_module_ext: %d - %d",*num, moudles_num);
}
