#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>

#include "eng_vector.h"
#include "CChnlMgr.h"
#include "config.h"

#define stricmp strcasecmp

#define MAX_LEN 1024

CChnlMgr::CChnlMgr(CChnlThreadMgr* tMgr, CDevMgr* dMgr, CModuleMgr* mMgr){
    m_lpChnlThreadMgr = tMgr;
    m_lpChnlDevMgr = dMgr;
    m_lpModMgr = mMgr;
}

CChnlMgr::~CChnlMgr(){
}

int CChnlMgr::load(char* dirpath){
    if (dirpath == NULL){
        return -1;
    }

    DIR *dir = NULL;
    struct dirent *ptr = NULL;
    char path[MAX_LEN]=" ";
    if ((dir = opendir(dirpath)) == NULL)
    {
        error("Open %s error...%s",dirpath,strerror(errno));
        return -1;
    }

    while ((ptr = readdir(dir)) != NULL)
    {
        if (ptr->d_type == 8 && strstr(ptr->d_name, ".conf") != NULL) { /// file
            EngConf *lpCnf = new EngConf();
            snprintf(path, sizeof(path), "%s/%s", dirpath, ptr->d_name);
            if (0 == lpCnf->parse(EngConf::CONF_CHNL, path)){
                char chnlName[BOOTMODE_NAME_LEN] = {0};
                CChnl *lpChnl = NULL;
                char* name=NULL, *sDev=NULL, *sPort=NULL, *dDev=NULL, *dPort=NULL;
                char* act=NULL, *datatype=NULL, *enable=NULL, *ap_process=NULL, *reserved=NULL, *desc=NULL;

                strncpy(chnlName, ptr->d_name, strlen(ptr->d_name)-strlen(".conf"));
                info("chnl name: %s", chnlName);
                lpChnl = new CChnl(chnlName, m_lpChnlThreadMgr, m_lpChnlDevMgr, m_lpModMgr);

                for(int i = 0; i < lpCnf->m_cnt.size(); i++){
                    int j = 0;
                    EngVector<char*>* pitem = lpCnf->m_cnt[i];
                    //chnl name
                    if (j < (*pitem).size()){
                        name = (*pitem)[j++];
                    }
                    //src dev
                    if (j < (*pitem).size()){
                        sDev = (*pitem)[j++];
                    }
                    //src port
                    if (j < (*pitem).size()){
                        sPort = (*pitem)[j++];
                    }
                    //dst dev
                    if (j < (*pitem).size()){
                        dDev = (*pitem)[j++];
                    }
                    //dst port
                    if (j < (*pitem).size()){
                        dPort = (*pitem)[j++];
                    }
                    //act
                    if (j < (*pitem).size()){
                        act = (*pitem)[j++];
                    }
                    //datatype
                    if (j < (*pitem).size()){
                        datatype = (*pitem)[j++];
                    }
                    //enable
                    if (j < (*pitem).size()){
                        enable = (*pitem)[j++];
                    }
                    //ap_process
                    if (j < (*pitem).size()){
                        ap_process = (*pitem)[j++];
                    }
                    //reserved
                    if (j < (*pitem).size()){
                        reserved = (*pitem)[j++];
                    }
                    //desc
                    if (j < (*pitem).size()){
                        desc = (*pitem)[j++];
                    }

                    lpChnl->add(name, sDev, sPort, dDev, dPort, act, datatype, enable, ap_process, reserved, desc);
                }
                addChnl(lpChnl);
            }
        }
    }

    closedir(dir);

    return 0;
}

int CChnlMgr::addChnl(CChnl* chnl){
    CChnl* pChnl = find(chnl->getname());
    if(pChnl != NULL){
        return -1;
    }

    m_chnlList.push_back(chnl);

    return 0;
}

CChnl* CChnlMgr::find(const char* name){
    if (name == NULL){
        return NULL;
    }

    for(int i = 0; i < m_chnlList.size(); i++){
        if(stricmp(m_chnlList[i]->getname(), name) == 0){
            return m_chnlList[i];
        }
    }

    return NULL;
}

bool CChnlMgr::enable(const char *bootmode, bool enable){
    if (bootmode == NULL){
        return false;
    }

    CChnl* nl = find(bootmode);
    if (nl != NULL){
        nl->enable(enable);
    }

    return true;
}

void CChnlMgr::run(const char* bootmode){
    CChnl* pChnl = find((char*)bootmode);
    if(pChnl == NULL){
        error("can't find bootmode = %s", bootmode);
        return ;
    }

    pChnl->run();
}

void CChnlMgr::resetWithChnlName(const char* chnlname){
}

void CChnlMgr::resetWithDevName(const char* devname, bool enable){
    CDev* lpDev = m_lpChnlDevMgr->find((char*)devname);
    if (lpDev != NULL){
        lpDev->enablePortRD(enable);
        lpDev->enablePortWR(enable);

        //cp com at alway enable true
        lpDev->enablePort(DEV_MODEM_AT_NAME, true, true);
    }

    m_lpChnlThreadMgr->restartWithDev(devname);
}

void CChnlMgr::resetWithPortName(const char* devname, const char* portname){
}


void CChnlMgr::print(){
    for(int i = 0; i < m_chnlList.size(); i++){
        CChnl *nl = m_chnlList[i];
        info("bootmode = %s", nl->getname());
        nl->print();
    }
}