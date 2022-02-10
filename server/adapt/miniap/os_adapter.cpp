#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <cutils/properties.h>
#include <log/log.h>
#include <sys/syscall.h>

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
    strcpy(buff, "none");

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
    return 1;
}

void OS_usb_vser_enable(int mode){

}
int wait_for_usbenum_succ(int timeout, char *state)
{
    return 0;
}

void OS_sys_getlogdest(char* log_type, char* location){
    char log_dest[PROPERTY_VALUE_MAX] = {0};

    if (*log_type == '1'){
        *location = '1';
    }else if (*log_type == '2'){
        *location = '1';
    }else{
    }
}

void OS_sys_setlogdest(char* log_type, char* location){
    char log_dest[PROPERTY_VALUE_MAX] = {0};

    if (*log_type == '1'){
        log_dest[0] = *location;
    }else if (*log_type == '2'){
        log_dest[0] = *location;
    }else{
    }
}

void OS_sys_restart_self(){
}

#define gettid() syscall(SYS_gettid)
int OS_sys_gettid(){
    return gettid();
}