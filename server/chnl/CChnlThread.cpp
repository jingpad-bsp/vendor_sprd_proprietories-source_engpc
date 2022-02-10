#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>

#include "channel.h"
#include "dev.h"
#include "englog.h"
#include "CTrans.h"
#include "CDataDiag.h"
#include "CDataAT.h"
#include "CDataLog.h"
#include "CDataAgdsp.h"
#include "CChnlThread.h"
#include "CChnlThreadMgr.h"

CChnlThread::CChnlThread(CModuleMgr* mMgr, char* name, CPort* lpSrc, CPort* lpDst, int enable, int dataType, int apProcess){
    m_lpModMgr = mMgr;
     m_lpChnlTdMgr = NULL;
    m_lpPortSrc = lpSrc;
    m_lpPortDst = lpDst;
    m_dataType = dataType;
    m_enable = enable;
    m_apProcess = apProcess;
    m_isRunning = false;
    strcpy(m_name, name);
    m_lpTrans = NULL;
}

CChnlThread::~CChnlThread(){
}

CPort* CChnlThread::getSrcPort(){
    return m_lpPortSrc;
}

CPort* CChnlThread::getDstPort(){
    return m_lpPortDst;
}

void CChnlThread::addDstPort(CPort* lpDstPort){
    if (lpDstPort != NULL){
        m_ListDstPort.push_back(lpDstPort);
    }
}

void CChnlThread::activeDstPort(CPort* lpDstPort){
    m_lpPortDst = lpDstPort;
    if (m_lpTrans != NULL){
        m_lpTrans->updatePortDst(lpDstPort);
    }
}

CChnlThread* CChnlThread::findDstPort(CPort* lpDstPort){
    for(int i = 0; i < m_ListDstPort.size(); i++){
        if (m_ListDstPort[i] == lpDstPort){
            return this;
        }
    }

    return NULL;
}

int CChnlThread::getDataType(){
    return m_dataType;
}

int CChnlThread::getApProcess(){
    return m_apProcess;
}

int CChnlThread::getThreadId(){
    return m_idThread;
}

char* CChnlThread::getName(){
    return m_name;
}

bool CChnlThread::isRunning(){
    return m_isRunning;
}

bool CChnlThread::stop(){
    if (m_lpPortSrc != NULL){
        m_lpPortSrc->stop();
    }
    return true;
}

bool CChnlThread::run(){
    if (m_enable == 0){
        return false;
    }

    pthread_attr_t attr;
    pthread_attr_init(&attr);

    return pthread_create(&m_idThread, &attr, threadMain, (void*)this);
}

void* CChnlThread::threadMain(void *arg){
    CChnlThread* lpThread = (CChnlThread*)arg;
    CTrans* lpTrans = NULL;

    if (lpThread->m_dataType == DATA_DIAG || lpThread->m_dataType == DATA_WCN_DIAG){
        lpTrans = new CDataDiag(lpThread->getModMgr());
    }else if(lpThread->m_dataType == DATA_AT||lpThread->m_dataType == DATA_WCN_AT){
        lpTrans = new CDataAT(lpThread->getModMgr());
    }else if(lpThread->m_dataType == DATA_LOG){
        lpTrans = new CDataLog(lpThread->getModMgr());
    }else if(lpThread->m_dataType == DATA_AGDSP_LOG || lpThread->m_dataType == DATA_AGDSP_PCM || lpThread->m_dataType == DATA_AGDSP_MEM){
        lpTrans = new CDataAgdsp(lpThread->getModMgr());
    }else{
        return NULL;
    }

    lpThread->m_isRunning = true;
    lpThread->m_lpTrans = lpTrans;
    lpTrans->init(lpThread->m_name, lpThread->m_lpPortSrc, lpThread->m_lpPortDst, lpThread->m_dataType, lpThread->m_apProcess);
    lpTrans->attach(lpThread);
    while(-1 == lpTrans->trans()){
        break;
    }

    lpThread->m_isRunning = false;

    delete lpTrans;
    lpTrans = NULL;
    lpThread->m_lpTrans = NULL;

    return NULL;
}
