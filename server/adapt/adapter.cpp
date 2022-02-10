#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include "os_adapter.h"
#include "adapter.h"

const char* usb_getEnablePath(){
    return OS_usb_getEnablePath();
}

const char* usb_getMaxSpeedPath(){
    return OS_usb_getMaxSpeedPath();
}

const char* usb_getStatePath(){
    return OS_usb_getStatePath();
}

int usb_getConf(char* buff, int nlen){
    return OS_usb_getConf(buff, nlen);
}

void usb_vser_enable(int mode){
    OS_usb_vser_enable(mode);
}

const char* getChargeStopPath(){
    return OS_getChargeStopPath();
}

int is_data_dir_ready(){
    return OS_is_data_dir_ready();
}

void sys_getlogdest(char* log_type, char* location){
    OS_sys_getlogdest(log_type, location);
}

void sys_setlogdest(char* log_type, char* location){
    OS_sys_setlogdest(log_type, location);
}

void sys_restart_self(){
    OS_sys_restart_self();
}

int sys_gettid(){
    return OS_sys_gettid();
}