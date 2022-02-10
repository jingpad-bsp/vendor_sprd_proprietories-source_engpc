#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "eng_vector.h"

#include "CDev.h"

#define stricmp strcasecmp

CDev::CDev(char* name){
    strcpy(m_devName, name);
    memset(m_curMode, 0, sizeof(m_curMode));
}

CDev::~CDev(){
}

char* CDev::getname(){
    return m_devName;
}

int CDev::activeMode(char* bootmode){
    if (bootmode == NULL){
        return -1;
    }

    for(int i = 0; i < m_BMDevList.size(); i++){
        if (stricmp(m_BMDevList[i]->bootmode, bootmode) == 0){
            strcpy(m_curMode, bootmode);
            return 0;
        }
    }

    return -1;
}

int CDev::addPort(char* bootmode, CPort* port){
    if (bootmode == NULL){
        return -1;
    }

    BMDev* lpDev = findBootMode(bootmode);
    if (lpDev == NULL){
        BMDev *lpNewDev = new BMDev();
        strcpy(lpNewDev->bootmode, bootmode);
        lpNewDev->portList.push_back(port);

        m_BMDevList.push_back(lpNewDev);
    }else{
        lpDev->portList.push_back(port);
    }

    return 0;
}

int CDev::rmPort(char* bootmode, CPort* pPort){
    // to be continue
    return 0;
}

int CDev::rmPort(char* bootmode, char* name){
    // to be continue
    return 0;
}

CPort* CDev::find(char* name){
    return find(m_curMode, name);
}

CPort* CDev::find(char* bootmode, char* name){
    if (bootmode == NULL || name == NULL){
        return NULL;
    }

    for(int i = 0; i < m_BMDevList.size(); i++){
        if (stricmp(m_BMDevList[i]->bootmode, bootmode) == 0){
            for(int j = 0; j < m_BMDevList[i]->portList.size(); j++){
                CPort* lpPort = m_BMDevList[i]->portList[j]->find(name);
                if ( lpPort != NULL){
                    return lpPort;
                }
            }
        }
    }

    return NULL;
}

CDev::BMDev* CDev::findBootMode(char* bootmode){
    if (bootmode == NULL){
        return NULL;
    }

    for(int i = 0; i < m_BMDevList.size(); i++){
        if (stricmp(m_BMDevList[i]->bootmode, bootmode) == 0){
            return m_BMDevList[i];
        }
    }

    return NULL;
}

bool CDev::enable(bool enable){
    m_enable = enable;
    return true;
}

bool CDev::enablePort(char *name, bool enableRD, bool enableWR){
    CPort* lpPort = find(name);
    if (lpPort == NULL){
        return false;
    }

    return lpPort->enable(enableRD, enableWR);
}

bool CDev::enablePortRD(bool enableRD){
    for(int i = 0; i < m_BMDevList.size(); i++){
        if (stricmp(m_BMDevList[i]->bootmode, m_curMode) == 0){
            for(int j = 0; j < m_BMDevList[i]->portList.size(); j++){
                bool enRD = false;
                bool enWR = false;
                CPort* lpPort = m_BMDevList[i]->portList[j];
                lpPort->getEnable(enRD, enWR);
                lpPort->enable(enableRD, enWR);
            }
        }
    }

    return true;
}

bool CDev::enablePortWR(bool enableWR){
    for(int i = 0; i < m_BMDevList.size(); i++){
        if (stricmp(m_BMDevList[i]->bootmode, m_curMode) == 0){
            for(int j = 0; j < m_BMDevList[i]->portList.size(); j++){
                bool enRD = false;
                bool enWR = false;
                CPort* lpPort = m_BMDevList[i]->portList[j];
                lpPort->getEnable(enRD, enWR);
                lpPort->enable(enRD, enableWR);
            }
        }
    }

    return true;
}

int CDev::open(char* name){
    CPort* lpPort = find(name);
    if (lpPort == NULL){
        return -1;
    }

    return lpPort->open();
}

int CDev::close(char* name){
    CPort* lpPort = find(name);
    if (lpPort == NULL){
        return -1;
    }

    return lpPort->close();
}

int CDev::read(char* name, char* buff, int nLen){
    CPort* lpPort = find(name);
    if (lpPort == NULL){
        return -1;
    }

    return lpPort->read(buff, nLen);
}

int CDev::write(char* name, char* buff, int nLen){
    CPort* lpPort = find(name);
    if (lpPort == NULL){
        return -1;
    }

    return lpPort->write(buff, nLen);
}

void CDev::print(){
    info("size = %d", m_BMDevList.size());
    for(int i = 0; i < m_BMDevList.size(); i++){
        info("bootmode = %s", m_BMDevList[i]->bootmode);
        for(int j = 0; j < m_BMDevList[i]->portList.size(); j++){
            CPort* lpPort = m_BMDevList[i]->portList[j];
            lpPort->print();
        }
    }
}