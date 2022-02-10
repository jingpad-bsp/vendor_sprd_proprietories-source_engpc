#ifndef __ENGPC_BOOTMODE_H__
#define __ENGPC_BOOTMODE_H__

#include "usb.h"

#define BOOTMODE_CALI               "cali"
#define BOOTMODE_AUTOTEST           "autotest"
#define BOOTMODE_FACTORYTEST        "factorytest"
#define BOOTMODE_NORMAL             "normal"
#define BOOTMODE_NORMAL_LITE        "normal_lite"

const char* initBootMode();
const char* parseCmdline();

#endif