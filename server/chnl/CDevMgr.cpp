#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>

#include "eng_vector.h"
#include "CDevMgr.h"
#include "config.h"
#include "englog.h"

#define MAX_LEN 1024

#define stricmp strcasecmp

CDevMgr::CDevMgr(){
}

CDevMgr::~CDevMgr(){
}

int CDevMgr::notify(UEVENT_MSG msg, void* param){
    CDevMgr* lpDevMgr = (CDevMgr*)param;
    if (lpDevMgr == NULL){
        EngLog::error("dev mgr is NULL.");
        return -1;
    }

    if (msg == USB_CONNECT){
        CDev* lpDev = lpDevMgr->find(DEV_HOST_NAME);
        if (lpDev != NULL){
            lpDev->enablePortWR(true);
        }
    }else if (msg == USB_DISCONNECT){
        CDev* lpDev = lpDevMgr->find(DEV_HOST_NAME);
        if (lpDev != NULL){
            lpDev->enablePortWR(false);
        }
    }else{
    }

    return 0;
}

int CDevMgr::load(char* dirpath){
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
        if (ptr->d_type == 8 && strstr(ptr->d_name, ".conf") != NULL ) { /// file
            EngConf *lpCnf = new EngConf();
            snprintf(path, sizeof(path), "%s/%s", dirpath, ptr->d_name);
            if (0 == lpCnf->parse(EngConf::CONF_DEV, path)){
                char* bootmode=NULL,*name=NULL,*data_type=NULL,*port_type=NULL,*port_path=NULL,*reserved=NULL,*description=NULL;
                char devName[DEV_NAME_LEN] = {0};
                CDev *lpDev = NULL;

                strncpy(devName, ptr->d_name, strlen(ptr->d_name)-strlen(".conf"));
                info("dev name: %s", devName);
                lpDev = new CDev(devName);
                info("size = %d", lpCnf->m_cnt.size());
                for(int i = 0; i < lpCnf->m_cnt.size(); i++){
                    int j = 0;
                    EngVector<char*>* pitem = lpCnf->m_cnt[i];
                    //bootmode
                    if (j < (*pitem).size()){
                        bootmode = (*pitem)[j++];
                    }
                    //name
                    if (j < (*pitem).size()){
                        name = (*pitem)[j++];
                    }
                    //data_type
                    if (j < (*pitem).size()){
                        data_type = (*pitem)[j++];
                    }
                    //port_type
                    if (j < (*pitem).size()){
                        port_type = (*pitem)[j++];
                    }
                    //port_path
                    if (j < (*pitem).size()){
                        port_path = (*pitem)[j++];
                    }
                    //reserved
                    if (j < (*pitem).size()){
                        reserved = (*pitem)[j++];
                    }
                    //description
                    if (j < (*pitem).size()){
                        description = (*pitem)[j++];
                    }

                    lpDev->addPort(bootmode, new CPort(devName, name, port_type, port_path, data_type, reserved, description));
                }
                addDev(lpDev);
            }
        }
    }

    closedir(dir);

    return 0;
}

int CDevMgr::addDev(CDev* dev){
    CDev* pdev = find(dev->getname());
    if(pdev != NULL){
        return -1;
    }

    m_devList.push_back(dev);

    return 0;
}

int CDevMgr::rmDev(CDev* dev){
    return 0;
}

int CDevMgr::rmDev(char* name){
    return 0;
}

CDev* CDevMgr::find(char* name){
    if (name == NULL){
        return NULL;
    }

    for(int i = 0; i < m_devList.size(); i++){
        if(stricmp(m_devList[i]->getname(), name) == 0){
            return m_devList[i];
        }
    }

    return NULL;
}

bool CDevMgr::enable(char *name, bool enable){
    if (name == NULL){
        return false;
    }

    CDev* lpDev = find(name);
    if (lpDev != NULL){
        return lpDev->enable(enable);
    }

    return false;
}

void CDevMgr::activeMode(char* bootmode){
    info("active mode: %s", bootmode);
    for(int i = 0; i < m_devList.size(); i++){
        if (0 != m_devList[i]->activeMode(bootmode)){
            m_devList[i]->activeMode(DEF_BOOT_MODE);
        }
    }
}

void CDevMgr::print(){
    info("print..size = %d", m_devList.size());
    for(int i = 0; i < m_devList.size(); i++){
        info("dev name: %s", m_devList[i]->getname());
        m_devList[i]->print();
    }
}
