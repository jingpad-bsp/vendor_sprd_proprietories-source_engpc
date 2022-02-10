#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>

#include "CChnl.h"
#include "dev.h"
#include "CChnlThread.h"

#define stricmp strcasecmp

CChnl::CChnl(char* bootmode, CChnlThreadMgr* tMgr, CDevMgr* dMgr, CModuleMgr* mMgr){
    strcpy(m_bootMode, bootmode);
    m_lpChnlThreadMgr = tMgr;
    m_lpChnlDevMgr = dMgr;
    m_lpModMgr = mMgr;
    m_bEnable = true;
}

CChnl::~CChnl(){
}

int CChnl::add(char* name, char* sDev, char* sPort, char* dDev, char* dPort, int act, 
        int dataType, int enable, int ap_process, char* reserved, char* desc){

    CHNL_PROP *pChnlProp = (CHNL_PROP *)malloc(sizeof(CHNL_PROP));
    if (pChnlProp == NULL){
        return -1;
    }

    strcpy(pChnlProp->name, name);
    strcpy(pChnlProp->srcDevName, sDev);
    strcpy(pChnlProp->srcPortName, sPort);
    strcpy(pChnlProp->dstDevName, dDev);
    strcpy(pChnlProp->dstPortName, dPort);
    pChnlProp->act = act;
    pChnlProp->dataType = (DATA_TYPE)dataType;
    pChnlProp->enable = enable;
    pChnlProp->ap_process = ap_process;
    strcpy(pChnlProp->reserved, reserved);
    strcpy(pChnlProp->desc, desc);

    m_chnlList.push_back(pChnlProp);

    return 0;
}

int CChnl::add(char* name, char* sDev, char* sPort, char* dDev, char* dPort, char* act, 
        char* dataType, char* enable, char* ap_process, char* reserved, char* desc){

    return add(name, sDev, sPort, dDev, dPort, str2act(act), 
            str2DataType(dataType), str2enable(enable), str2approcess(ap_process), reserved, desc);
}

char* CChnl::getname(){
    return m_bootMode;
}

void CChnl::enable(bool enable){
    m_bEnable = enable;
}

bool CChnl::enable(char* name, bool enable){
    if (name == NULL){
        return false;
    }

    CHNL_PROP* pItem = find(name);
    if (pItem != NULL){
        pItem->enable = enable;
        return true;
    }

    return false;
}

CHNL_PROP* CChnl::find(char* name){
    if (name == NULL){
        return NULL;
    }

    for(int i = 0; i < m_chnlList.size(); i++){
        CHNL_PROP* pItem = m_chnlList[i];
        if (stricmp(pItem->name, name)){
            return pItem;
        }
    }

    return NULL;
}

void CChnl::run(){
    info("run..size = %d", m_chnlList.size());
    for(int i = 0; i < m_chnlList.size(); i++){
        if ( !runInThread(m_chnlList[i])){
            error("thread: %s fail!", m_chnlList[i]->name);
        }
    }
}

void CChnl::stop(){
    info("stop..size = %d", m_chnlList.size());
}

bool CChnl::runInThread(CHNL_PROP *lpChnlProp){
    info("runInThread");
    if (m_bEnable == false){
        error("enable is false.");
        return false;
    }

    if (lpChnlProp == NULL){
        error("can't find chnl prop.");
        return false;
    }

    if (m_lpChnlThreadMgr == NULL){
        error("invalid m_lpDevMgr");
        return false;
    }

    if (m_lpChnlDevMgr == NULL){
        error("invalid m_lpDevMgr");
        return false;
    }

    info("act = %s ", act2str(lpChnlProp->act));
    if (WORK_MODE_S == lpChnlProp->act || WORK_MODE_F == lpChnlProp->act){
        CDev* lpSrcDev = m_lpChnlDevMgr->find(lpChnlProp->srcDevName);
        if (lpSrcDev == NULL){
            warn("invalid dev: %s", lpChnlProp->srcDevName);
            return false;
        }
        CPort *lpSrcPort = lpSrcDev->find(lpChnlProp->srcPortName);
        if (lpSrcPort == NULL){
            warn("invalid port: %s", lpChnlProp->srcPortName);
            return false;
        }
        CDev* lpDstDev = m_lpChnlDevMgr->find(lpChnlProp->dstDevName);
        if (lpDstDev == NULL){
            warn("invalid dev: %s", lpChnlProp->dstDevName);
            return false;
        }
        CPort *lpDstPort = lpDstDev->find(lpChnlProp->dstPortName);
        if (lpDstPort == NULL){
            warn("invalid port: %s", lpChnlProp->dstPortName);
            return false;
        }

        info("name = %s, src = %s, dst = %s, dataType = %s, ap_process = %d", 
             lpChnlProp->name, lpSrcPort->getname(), lpDstPort->getname(), DataType2str(lpChnlProp->dataType), lpChnlProp->ap_process);

        int ap_process = (lpChnlProp->ap_process == 1) || (lpChnlProp->ap_process == 3)?1:0;
        CChnlThread *td = m_lpChnlThreadMgr->findSameSrcPort(lpSrcPort);
        if ( td != NULL){
            info("%s: add dest port", td->getName());
            td->addDstPort(lpDstPort);
        }else{
            td = new CChnlThread(m_lpModMgr, lpChnlProp->name, lpSrcPort, lpDstPort, lpChnlProp->enable, lpChnlProp->dataType, ap_process);
            td->addDstPort(lpDstPort);
            td->run();
            m_lpChnlThreadMgr->add(td);
        }

        if (WORK_MODE_F == lpChnlProp->act){
            char buff[CHNL_NAME_LEN+2] = "0";
            sprintf(buff, "%s-R", lpChnlProp->name);
            ap_process = (lpChnlProp->ap_process == 2) || (lpChnlProp->ap_process == 3)?1:0;
            CChnlThread *tdF = m_lpChnlThreadMgr->findSameSrcPort(lpDstPort);
            if (tdF != NULL){
                info("%s: add dest port", td->getName());
                tdF->addDstPort(lpSrcPort);
            }else{
                tdF = new CChnlThread(m_lpModMgr, buff, lpDstPort, lpSrcPort, lpChnlProp->enable, lpChnlProp->dataType, ap_process);
                tdF->addDstPort(lpSrcPort);
                tdF->run();
                m_lpChnlThreadMgr->add(tdF);
            }
        }

    }else{
        error("invalid act");
    }

    return true;
}

void CChnl::print(){
    for(int i = 0; i < m_chnlList.size(); i++){
        CHNL_PROP* lpProp = m_chnlList[i];
        info(" %s, %s, %s, %s, %s, %d, %s, %d, %d, %s, %s",
             lpProp->name, lpProp->srcDevName, lpProp->srcPortName, lpProp->dstDevName, lpProp->dstPortName,
             lpProp->act, DataType2str(lpProp->dataType), lpProp->enable, lpProp->ap_process, lpProp->reserved, lpProp->desc);
    }
}