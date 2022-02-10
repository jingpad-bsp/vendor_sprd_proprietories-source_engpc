#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <cutils/properties.h>
#include <log/log.h>

#include "os_adapter.h"

const char* OS_usb_getEnablePath(){
    if (0 == access(SYS_CLASS_ANDUSB_ENABLE, F_OK)){
        return SYS_CLASS_ANDUSB_ENABLE;
    }else if (0 == access(SYS_CLASS_ANDUSB_ENABLE_NEW, F_OK)){
        return SYS_CLASS_ANDUSB_ENABLE_NEW;
    }

    return NULL;
}

const char* OS_usb_getMaxSpeedPath(){
    if (0 == access(DEVICE_SOC_USB_MAXIMUM_SPEED, F_OK)){
        return DEVICE_SOC_USB_MAXIMUM_SPEED;
    }else if (0 == access(DEVICE_SOC_USB_MAXIMUM_SPEED_NEW, F_OK)){
        return DEVICE_SOC_USB_MAXIMUM_SPEED_NEW;
    }

    return NULL;
}

const char* OS_usb_getStatePath(){
    if (0 == access(SYS_CLASS_ANDUSB_STATE, F_OK)){
        return SYS_CLASS_ANDUSB_STATE;
    }

    return NULL;
}

int OS_usb_getEnableCmd(char* buff, int nlen){
    char cmd[128] = {0};
    property_get("sys.usb.controller", cmd, "not_find");
    if (strcmp(cmd, "not_find") == 0) {
        strcpy(buff, "1");
    }else{
        strcpy(buff, cmd);
    }

    return 0;
}

int OS_usb_getConf(char* buff, int nlen){
    property_get("persist.vendor.sys.modem.diag", buff, "none");

    return 0;
}

const char* OS_getChargeStopPath(){
    if (0 == access(CHARGER_STOP_PATH, F_OK)){
        return CHARGER_STOP_PATH;
    }else if (0 == access(CHARGER_STOP_NEW_PATH, F_OK)){
        return CHARGER_STOP_NEW_PATH;
    }

    return NULL;
}

int OS_is_data_dir_ready(){
    int ready = 0;
    char crypto_type[PROPERTY_VALUE_MAX] = { 0 };
    char crypto_state[PROPERTY_VALUE_MAX] = { 0 };
    char vold_decrypt[PROPERTY_VALUE_MAX] = { 0 };
    property_get("ro.crypto.type", crypto_type, "");
    property_get("ro.crypto.state", crypto_state, "");
    property_get("vold.decrypt", vold_decrypt, "");
    if (strcmp(crypto_type, "block") == 0 && strcmp(crypto_state, "encrypted") == 0){
        if (strcmp(vold_decrypt, "trigger_restart_framework") == 0){
            ready = 1;
        }else{
            ready = 0;
            ALOGD("fde ,data is not read,vold_decrypt is %s", vold_decrypt);
        }
    }
    return ready;
}

void android_set_vendor_bootmode(){
    char buff[PROPERTY_VALUE_MAX] = {0};
    property_get(SYSTEM_PROP_BOOTMODE, buff, "");
    ALOGD("ENGPC: %s = %s", SYSTEM_PROP_BOOTMODE, buff);
    property_set(VENDOR_PROP_BOOTMODE, buff);
}

void OS_usb_vser_enable(int mode){
    char cmd[64] = {0};
    char path[128] = {0};
    int fd, ret;

    android_set_vendor_bootmode();

    ALOGD("ENGPC: vser enable: %d", mode);
    if (mode == 0){//autotest
        property_set(PROP_USB_CONFIG, PROP_USB_CONFIG_VALUE);
        wait_for_usbenum_succ(600, USB_CONFIG_VSER);
        //usleep(2000*1000);
    }else if(mode == 2){//caliroca
        property_set(PROP_USB_CONFIG, PROP_USB_CONFIG_VALUE);
        wait_for_usbenum_succ(600, USB_CONFIG_VSER_GSER);
            //usleep(2000*1000);
    }else if (mode == 1){//cali
        const char* path = OS_usb_getEnablePath();
        if (path == NULL) {
            ALOGD("ENGPC: invalid usb enable path.");
            return ;
        }

        ALOGD("ENGPC: usb enable path: %s", path);
        OS_usb_getEnableCmd(cmd, sizeof(cmd));
        ALOGD("ENGPC: usb enable cmd: %s", cmd);

        fd = open(path, O_WRONLY);
        if (fd >= 0) {
            ret = write(fd, cmd, strlen(cmd));
            ALOGD("ENGPC: %s: Write sys class androidusb enable file: %d\n", __FUNCTION__, ret);
            close(fd);
        } else {
            ALOGD("ENGPC: %s: Open sys class androidusb enable file failed!\n", __FUNCTION__);
        }
    }
}
int wait_for_usbenum_succ(int timeout, char *state)
{
    int try_cnt = timeout;
    char prop[PROPERTY_VALUE_MAX] = {0};

    if (state == NULL || timeout <= 0) return -1;

    ALOGD("ENGPC: %s: timeout = %d, state = %s", __FUNCTION__, timeout, state);
    do {
        property_get(PROP_USB_STATE, prop, "not_find");
        ALOGD("ENGPC: %s %s=%s ", __FUNCTION__, PROP_USB_STATE, prop);

        if (!strcmp(prop, state)){
            ALOGD("ENGPC: %s: succ", __FUNCTION__);
            return 0;
        }
        usleep(100 * 1000);
    } while (try_cnt-- > 0);

    return -1;
}

void OS_sys_getlogdest(char* log_type, char* location){
    char log_dest[PROPERTY_VALUE_MAX] = {0};

    if (*log_type == '1'){
        property_get(PROP_MODEM_LOG_DEST, log_dest, "not_find");
        *location = log_dest[0];
    }else if (*log_type == '2'){
        property_get(PROP_WCN_LOG_DEST, log_dest, "not_find");
        *location = log_dest[0];
    }else{
    }
}

void OS_sys_setlogdest(char* log_type, char* location){
    char log_dest[PROPERTY_VALUE_MAX] = {0};

    if (*log_type == '1'){
        log_dest[0] = *location;
        property_set(PROP_MODEM_LOG_DEST, location);
    }else if (*log_type == '2'){
        log_dest[0] = *location;
        property_set(PROP_WCN_LOG_DEST, location);
    }else{
    }
}

void OS_sys_restart_self(){
    property_set(PROP_RESTART_SELF, "1");
    property_set(PROP_RESTART_SELF, "0");
}

int OS_sys_gettid(){
    return 0;
}
