#ifndef __CHNL__H__
#define __CHNL__H__

#include <stdio.h>

#include "CBase.h"
#include "channel.h"
#include "CChnlThreadMgr.h"
#include "CModuleMgr.h"
#include "CDevMgr.h"

class CChnl:public CBase{
    public:
        CChnl(char* bootmode, CChnlThreadMgr* tMgr, CDevMgr* dMgr, CModuleMgr* mMgr);
        virtual ~CChnl();

        const char* TAG(){return m_bootMode;}

        int add(char* name, char* sDev, char* sPort, char* dDev, char* dPort, int act, 
                int dataType, int enable, int ap_process, char* reserved, char* desc);
        int add(char* name, char* sDev, char* sPort, char* dDev, char* dPort, char* act, 
                char* dataType, char* enable, char* ap_process, char* reserved, char* desc);

        void enable(bool enable);
        bool enable(char* name, bool enable);
        CHNL_PROP* find(char* name);
        char* getname();
        void print();

        void run();
        void stop();

    private:
        char m_bootMode[BOOTMODE_NAME_LEN];
        EngVector<CHNL_PROP *>   m_chnlList;
        CChnlThreadMgr* m_lpChnlThreadMgr;
        CDevMgr* m_lpChnlDevMgr;
        CModuleMgr* m_lpModMgr;
        bool m_bEnable;

        bool runInThread(CHNL_PROP *lpChnProp);
        void *chnlThreadMain(void *x);
};

#endif