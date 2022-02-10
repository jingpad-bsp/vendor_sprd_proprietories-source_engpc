#ifndef __CHNL_DEV_H__
#define __CHNL_DEV_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "eng_vector.h"
#include "CPort.h"
#include "CBase.h"

#define DEF_BOOTMODE "normal"

class CDev:public CBase {

    class BMDev{
        public:
            char    bootmode[BOOTMODE_NAME_LEN];
            EngVector<CPort *>   portList;
    };

    public:
        CDev(char* name);
        virtual ~CDev();

        const char* TAG(){sprintf(m_tag, "[%s][%s]", m_curMode, m_devName); return m_tag;}

        int activeMode(char* bootmode);

        BMDev* findBootMode(char* bootmode);
        CPort* find(char* name);
        CPort* find(char* bootmode, char* name);
        char* getname();

        int addPort(char* bootmode, CPort* pPort);

        bool enable(bool enable);
        bool enablePort(char *name, bool enableRD, bool enableWR);
        bool enablePortRD(bool enableRD);
        bool enablePortWR(bool enableWR);

        int open(char* name);
        int close(char* name);
        int read(char* name, char* buff, int nLen);
        int write(char* name, char* buff, int nLen);

        void print();

    private:
        char m_tag[BOOTMODE_NAME_LEN+DEV_NAME_LEN];
        char m_curMode[BOOTMODE_NAME_LEN];
        char m_devName[DEV_NAME_LEN];
        bool m_enable;

        EngVector<BMDev*>   m_BMDevList;

        int rmPort(char* bootmode, CPort* pPort);
        int rmPort(char* bootmode, char* name);
};

#endif