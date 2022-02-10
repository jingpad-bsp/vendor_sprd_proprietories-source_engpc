#ifndef __CHNL__THREAD__MGR__H__
#define __CHNL__THREAD__MGR__H__

#include "eng_vector.h"
#include "CPort.h"

class CChnlThread;

class CChnlThreadMgr{
    public:
        ~CChnlThreadMgr();

        static CChnlThreadMgr* getInstance(){
            return new CChnlThreadMgr();
        }

        void add(CChnlThread* lpThread);
        CChnlThread* findSameSrcPort(CPort* lpPort);
        CChnlThread* findSameDstPort(CPort* lpPort);
        void restartWithDev(const char* name);
        void enableChnl(const char* name, bool enable);
        void resetDstPortWithSameSrcPort(CPort* lpPort, CChnlThread* lpChnlThread);
    private:
        EngVector<CChnlThread* > m_threadList;
        CChnlThreadMgr();
};

#endif