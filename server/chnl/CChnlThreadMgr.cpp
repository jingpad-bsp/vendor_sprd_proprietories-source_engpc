#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>

#include "eng_vector.h"

#include "CPort.h"
#include "CChnlThread.h"
#include "CChnlThreadMgr.h"

#define stricmp strcasecmp

CChnlThreadMgr::CChnlThreadMgr(){
    m_threadList.clear();
}

CChnlThreadMgr::~CChnlThreadMgr(){
}

void CChnlThreadMgr::add(CChnlThread* lpThread){
    if (lpThread != NULL){
        m_threadList.push_back(lpThread);
        lpThread->attach(this);
    }
}

CChnlThread* CChnlThreadMgr::findSameSrcPort(CPort* lpPort){
    for(int i = 0; i < m_threadList.size(); i++){
        if (lpPort == m_threadList[i]->getSrcPort()){
            return m_threadList[i];
        }
    }

    return NULL;
}

CChnlThread* CChnlThreadMgr::findSameDstPort(CPort* lpPort){
    for(int i = 0; i < m_threadList.size(); i++){
        CChnlThread* lpTd = m_threadList[i]->findDstPort(lpPort);
        if (lpTd != NULL){
            return lpTd;
        }
    }

    return NULL;
}

void CChnlThreadMgr::resetDstPortWithSameSrcPort(CPort* lpPort, CChnlThread* lpChnlThread){
    for(int i = 0; i < m_threadList.size(); i++){
        if (lpPort == m_threadList[i]->getSrcPort() && lpChnlThread != m_threadList[i]){
            //active another port
        }
    }
}

void CChnlThreadMgr::restartWithDev(const char* name){
    for(int i = 0; i < m_threadList.size(); i++){
        CPort* lpPort = m_threadList[i]->getSrcPort();
        if(lpPort != NULL && stricmp(lpPort->getDevName(), name) == 0){
            CChnlThread* lpThread = m_threadList[i];
            if (!lpThread->isRunning()){
                lpThread->run();
            }
        }
    }
}

void CChnlThreadMgr::enableChnl(const char* name, bool enable){
    for(int i = 0; i < m_threadList.size(); i++){
        char* chnlname = m_threadList[i]->getName();
        if(chnlname != NULL && stricmp(chnlname, name) == 0){
            CChnlThread* lpThread = m_threadList[i];
            if (enable){
                if (!lpThread->isRunning()){
                    lpThread->run();
                }
            }else{
                lpThread->stop();
            }
        }
    }
}