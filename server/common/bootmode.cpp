#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

#include "bootmode.h"
#include "englog.h"
#include "usb.h"

const char* initBootMode(){
    char buff[64] = {0};
    const char* bootmode = parseCmdline();
    if (bootmode == NULL){
        eng_usb_config(buff, sizeof(buff));
        if (strcasecmp(buff, ",gser") == 0){
            return BOOTMODE_NORMAL;
        }else{
            return BOOTMODE_NORMAL_LITE;
        }
    }else{
        return bootmode;
    }
}

const char* parseCmdline(){
    char cmdline[1024] = {0};
    int ret = 0;
    char* str = NULL;
    int fd = open("/proc/cmdline", O_RDONLY);
    if (fd >= 0) {
        if ((ret = read(fd, cmdline, sizeof(cmdline) - 1)) > 0) {
            EngLog::debug("eng_pcclient: cmdline %s", cmdline);

            /*calibration*/
            str = strstr(cmdline, "androidboot.mode=cali");
            if (str != NULL) {
                close(fd);
                return BOOTMODE_CALI;
            }

            str = strstr(cmdline, "androidboot.mode=autotest");
            if (str != NULL) {
                close(fd);
                return BOOTMODE_AUTOTEST;
            }

            str = strstr(cmdline, "androidboot.mode=factorytest");
            if (str != NULL) {
                close(fd);
                return BOOTMODE_FACTORYTEST;
            }
        }

        close(fd);
    }

    return NULL;
}