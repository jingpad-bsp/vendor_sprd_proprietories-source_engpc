#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <getopt.h>
#include <ctype.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/reboot.h>
#include <cutils/android_reboot.h>
#include <bootloader_message/bootloader_message.h>
#include <cutils/properties.h>
#include <pthread.h>

#include "sprd_fts_type.h"
#include "sprd_fts_log.h"

//reboot to autodloader
#define AT_AUTODLOADER "AT+SPREF=\"AUTODLOADER\""

//reboot with cmd...
#define AT_REBOOT_CMD       "AT+SPREBOOTCMD="

//reboot to factory reset
#define AT_ETSRESET         "AT+ETSRESET"

//Power off after USB_DISCONNECT
#define USB_STATE_PAT  "/sys/class/android_usb/android0/state"

static char reboot_cmd_param[64] = {0};

void* autodloader_thread(void* arg) {
    usleep(2000*1000);
    android_reboot(ANDROID_RB_RESTART2, 0, "autodloader");
    return NULL;
}

void* reboot_cmd_thread(void* arg) {
    usleep(2000*1000);
    android_reboot(ANDROID_RB_RESTART2, 0, (char*)arg);
    return NULL;
}


int reboot_into_recovery(const std::vector<std::string>& options) {
    LOGD("Rebooting into recovery");
    std::string err;
    if (!write_bootloader_message(options, &err)) {
    LOGE("Rebooting into recovery fail!");
        return -1;
    }
    property_set("sys.powerctl", "reboot,recovery");
    return 0;
}
int phone_shutdown_new_API(void)
{
#ifdef LANGUAGE_CN
    const char Cmd1[] = "--wipe_data\n--locale=zh_CN";
#else
    const char Cmd1[] = "--wipe_data\n--locale=en_US";
#endif
    const char Cmd2[] = "--reason=wipe_data_via_recovery\n";
   LOGD("phone_shutdown_new_API Cmd1=%s,Cmd2=%s",Cmd1,Cmd2);
   return reboot_into_recovery({Cmd1, Cmd2});
}

int phone_reset_factorytest()
{
    return phone_shutdown_new_API();
}

void* reset_cmd_thread(void* arg) {
    sleep(2);
    phone_reset_factorytest();
    return NULL;
}

static int dloader_handle(char *buff, char *rsp)
{
    char *ptr = NULL;
    char cmd_buf[256] = {0};
    int ret = -1;
    if (NULL == buff)
    {
        ALOGE("%s,null pointer", __FUNCTION__);
        if(rsp != NULL){
            sprintf(rsp, "\r\nERROR\r\n");
            return strlen(rsp);
        }else{
            return 0;
        }
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

    if (strncasecmp(ptr, AT_AUTODLOADER,strlen(AT_AUTODLOADER)) == 0){
        //android_reboot(ANDROID_RB_RESTART2, 0, "autodloader");

        pthread_attr_t attr;
        pthread_t pthread;
        pthread_attr_init(&attr);
        pthread_create(&pthread, &attr, autodloader_thread, NULL);
        sprintf(rsp, "\r\nOK\r\n");
    }else{
        sprintf(rsp, "\r\nERROR\r\n");
    }
    if(buff[0] != 0x7e){
        free(ptr);
    }
    return strlen(rsp);
}

static int reboot_cmd_handle(char *buff, char *rsp)
{
    char *ptr = NULL;
    char cmd_buf[256] = {0};
    int ret = -1;
    int nlen = 0;
    if (NULL == buff)
    {
        ALOGE("%s,null pointer", __FUNCTION__);
        if(rsp != NULL){
            sprintf(rsp, "\r\nERROR\r\n");
            return strlen(rsp);
        }else{
            return 0;
        }
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

    if (strncasecmp(ptr, AT_REBOOT_CMD,strlen(AT_REBOOT_CMD)) == 0){
        char *ptrpara = ptr+strlen(AT_REBOOT_CMD);
        memset(reboot_cmd_param, 0, sizeof(reboot_cmd_param));
        nlen = strlen(buff)-strlen(AT_REBOOT_CMD);
        nlen = (nlen >= sizeof(reboot_cmd_param)-1)?(sizeof(reboot_cmd_param)-1):nlen;
        strncpy(reboot_cmd_param, ptr+strlen(AT_REBOOT_CMD), nlen);
        ENG_LOG("%s reboot param  = %s", __FUNCTION__, reboot_cmd_param);
        pthread_attr_t attr;
        pthread_t pthread;
        pthread_attr_init(&attr);
        pthread_create(&pthread, &attr, reboot_cmd_thread, reboot_cmd_param);
        sprintf(rsp, "\r\nOK\r\n");
    }else{
        sprintf(rsp, "\r\nERROR\r\n");
    }

    if(buff[0] != 0x7e){
        free(ptr);
        ptr = NULL;
    }

    return strlen(rsp);
}

static int reset_cmd_handle(char *buff, char *rsp)
{
    char *ptr = NULL;
    char cmd_buf[256] = {0};
    int ret = -1;
    int nlen = 0;
    if (NULL == buff)
    {
        ALOGE("%s,null pointer", __FUNCTION__);
        if(rsp != NULL){
            sprintf(rsp, "\r\nERROR\r\n");
            return strlen(rsp);
        }else{
            return 0;
        }
    }

    pthread_attr_t attr;
    pthread_t pthread;
    pthread_attr_init(&attr);
    pthread_create(&pthread, &attr, reset_cmd_thread, NULL);
    sprintf(rsp, "\r\nOK\r\n");

    return strlen(rsp);
}

int eng_usb_state(void) {
    int fd = -1;
    int ret = 0;
    char usb_state[32] = {0};
    fd = open(USB_STATE_PAT, O_RDONLY);
    if (fd >= 0) {
        ret = read(fd, usb_state, 32);
        if (ret > 0) {
            if (0 == strncmp(usb_state, "CONFIGURED", 10)
                || 0 == strncmp(usb_state, "CONNECTED", 9)) {
                ret = 1;
            } else {
                ret = 0;
            }
        }
        close(fd);
    } else {
        ret = 0;
    }
    return ret;
}

static int phone_poweroff(void)
{
    ENG_LOG("==== phone_poweroff enter ====\n");
    sync();
    android_reboot(ANDROID_RB_POWEROFF, 0, 0);
    return 0;
}

void *eng_power_off_thread(void *x) {
    while(1){
        int usb_plugin = eng_usb_state();
        if(!usb_plugin){
            //Do power off work while USB_DISCONNECT
            phone_poweroff();
            break;
        }
        usleep(100 * 1000);
    }
    return 0;
}
int eng_diag_poweroff(char *buf, int len, char *rsp, int rsp_len)
{
    MSG_HEAD_T *msg_head_ptr;
    int rlen = 0;

    memcpy(rsp, buf, sizeof(MSG_HEAD_T)+1);
    msg_head_ptr = (MSG_HEAD_T *)(rsp + 1);
    msg_head_ptr->len = sizeof(MSG_HEAD_T);
    msg_head_ptr->subtype = 0;

    char boot_mode[PROPERTY_VALUE_MAX] = {'\0'};
    property_get("ro.bootmode", boot_mode, NULL);
    ENG_LOG("%s boot_mode = %s", __FUNCTION__, boot_mode);
    if (!strcmp(boot_mode, "cali")){
        pthread_attr_t attr;
        pthread_t pthread;
        pthread_attr_init(&attr);
        pthread_create(&pthread, &attr, eng_power_off_thread, NULL);
    }else{
        phone_poweroff();
    }

    rsp[msg_head_ptr->len+1] = 0x7e;
    return msg_head_ptr->len+2;
}

#ifdef __cplusplus
extern "C"{
#endif

void register_this_module_ext(struct eng_callback *reg, int *num)
{
    int moudles_num = 0;
    ENG_LOG("register_this_module_ext :libdloader");

    sprintf((reg + moudles_num)->at_cmd, "%s", AT_AUTODLOADER);
    (reg + moudles_num)->eng_linuxcmd_func = dloader_handle;
    moudles_num++;

    sprintf((reg + moudles_num)->at_cmd, "%s", AT_REBOOT_CMD);
    (reg + moudles_num)->eng_linuxcmd_func = reboot_cmd_handle;
    moudles_num++;

    sprintf((reg + moudles_num)->at_cmd, "%s", AT_ETSRESET);
    (reg + moudles_num)->eng_linuxcmd_func = reset_cmd_handle;
    moudles_num++;

    (reg+moudles_num)->type = 0x11;
    (reg+moudles_num)->subtype = 0x0E;
    (reg+moudles_num)->eng_diag_func = eng_diag_poweroff;
    moudles_num++;

    *num = moudles_num;
    ENG_LOG("register_this_module_ext: %d - %d",*num, moudles_num);
}
#ifdef __cplusplus
}
#endif
