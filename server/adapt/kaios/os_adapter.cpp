#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <cutils/properties.h>

#include "os_adapter.h"

const char* OS_getUsbEnablePath(){
    if (0 == access(SYS_CLASS_ANDUSB_ENABLE, F_OK)){
        return SYS_CLASS_ANDUSB_ENABLE;
    }else if (0 == access(SYS_CLASS_ANDUSB_ENABLE_NEW, F_OK)){
        return SYS_CLASS_ANDUSB_ENABLE_NEW;
    }

    return NULL;
}

const char* OS_getChargeStopPath(){
    if (0 == access(CHARGER_STOP_PATH, F_OK)){
        return CHARGER_STOP_PATH;
    }else if (0 == access(CHARGER_STOP_NEW_PATH, F_OK)){
        return CHARGER_STOP_NEW_PATH;
    }

    return NULL;
}

const char* OS_getUsbMaxSpeedPath(){
    if (0 == access(DEVICE_SOC_USB_MAXIMUM_SPEED, F_OK)){
        return DEVICE_SOC_USB_MAXIMUM_SPEED;
    }else if (0 == access(DEVICE_SOC_USB_MAXIMUM_SPEED_NEW, F_OK)){
        return DEVICE_SOC_USB_MAXIMUM_SPEED_NEW;
    }

    return NULL;
}

const char* OS_getUsbStatePath(){
    if (0 == access(SYS_CLASS_ANDUSB_STATE, F_OK)){
        return SYS_CLASS_ANDUSB_STATE;
    }

    return NULL;
}

int OS_getUsbEnablCmd(char* buff, int nlen){
    char cmd[128] = {0};
    property_get("sys.usb.controller", cmd, "not_find");
    if (strcmp(cmd, "not_find") == 0) {
        strcpy(buff, "1");
    }else{
        strcpy(buff, cmd);
    }

    return 0;
}
