#ifndef __CHN_DEV_MGR_H__
#define __CHN_DEV_MGR_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "eng_vector.h"

#include "CDev.h"
#include "usb.h"


class CDevMgr:public CBase {
    public:
        virtual ~CDevMgr();

        static CDevMgr* getInstance(){
            return new CDevMgr();
        }

        const char* TAG(){return "CDevMgr";}

        int load(char* dir);
        CDev* find(char* name);

        bool enable(char *name, bool enable);
        void activeMode(char* bootmode);

        static int notify(UEVENT_MSG msg, void* param);

        void print();

    private:
        EngVector<CDev*> m_devList;

        CDevMgr();

        int addDev(CDev* dev);
        int rmDev(CDev* dev);
        int rmDev(char* name);
};

#endif