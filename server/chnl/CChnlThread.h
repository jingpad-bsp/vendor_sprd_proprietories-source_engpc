#ifndef __CHNL__THREAD__H__
#define __CHNL__THREAD__H__

#include <pthread.h>

#include "channel.h"
#include "CDevMgr.h"
#include "CPort.h"
#include "CModuleMgr.h"

class CChnlThreadMgr;

class CChnlThread{
    public:
        CChnlThread(CModuleMgr* mMgr, char* name, CPort* lpSrc, CPort* lpDst, int enable, int dataType, int apProcess);
        ~CChnlThread();

        CPort* getSrcPort();
        CPort* getDstPort();
        void addDstPort(CPort* lpDstPort);
        void activeDstPort(CPort* lpDstPort);
        CChnlThread* findDstPort(CPort* lpDstPort);
        int getDataType();
        int getApProcess();
        int getThreadId();
        char* getName();
        CModuleMgr* getModMgr() {return m_lpModMgr;}
        void attach(CChnlThreadMgr* lpChnlTdMgr){ m_lpChnlTdMgr = lpChnlTdMgr; }
        CChnlThreadMgr* getChnlThreadMgr(){ return m_lpChnlTdMgr; }

        bool run();
        bool stop();
        bool isRunning();

    private:
        char m_name[CHNL_NAME_LEN];
        CModuleMgr* m_lpModMgr;
        CChnlThreadMgr* m_lpChnlTdMgr;
        CPort* m_lpPortSrc;
        CPort* m_lpPortDst;
        int m_enable;
        int m_dataType;
        int m_apProcess;
        pthread_t m_idThread;
        bool m_isRunning;
        CTrans* m_lpTrans;

        EngVector<CPort *>   m_ListDstPort;

        static void* threadMain(void *);
};

#endif