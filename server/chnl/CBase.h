#ifndef  __ENG_CDEV_H__
#define  __ENG_CDEV_H__

#include <errno.h>
#include "dev.h"
#include "channel.h"

class CBase {
    public:
        CBase();
        virtual ~CBase();

        virtual const char* TAG() = 0;

        virtual int error(const char* fmt, ...);
        virtual int info(const char* fmt, ...);
        virtual int warn(const char* fmt, ...);
        virtual int debug(const char* fmt, ...);

        DATA_TYPE str2DataType(char* str);
        char* DataType2str(DATA_TYPE type);
        PORT_TYPE str2PortType(char* str);
        char* PortType2str(PORT_TYPE type);
        int str2act(char* act);
        char* act2str(int mode);
        int str2enable(char* enable);
        int str2approcess(char* ap_process);
};

#endif