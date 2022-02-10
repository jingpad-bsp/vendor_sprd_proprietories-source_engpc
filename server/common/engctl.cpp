#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <cutils/sockets.h>
#include <poll.h>
#include <pthread.h>
#include <sys/sysinfo.h>
#include <time.h>
#include <sys/time.h> 

#include "sprd_fts_type.h"

#include "engctl.h"
#include "englog.h"
#include "adapter.h"

CChnlMgr* CEngCtl::m_lpChnlMgr = NULL;

CEngCtl::CEngCtl(){
    m_lpChnlMgr = NULL;
    m_fdSockSrv = -1;

    memset((void*)&m_cmd_recv, 0, sizeof(m_cmd_recv));
    memset(m_cmd_rsp, 0, sizeof(m_cmd_rsp));
}

CEngCtl::~CEngCtl(){
}

void CEngCtl::attach(CChnlMgr* lpChnlMgr){
    m_lpChnlMgr = lpChnlMgr;
}

void CEngCtl::run(){
    pthread_t m_idEngCtl;
    pthread_attr_t attr;
    pthread_attr_init(&attr);

    if (0 != pthread_create(&m_idEngCtl, &attr, engCtl, (void*)this)){
        EngLog::error("cp log ctrl fail.");
    }
}

void* CEngCtl::engCtl(void *arg){
    CEngCtl* lpEngCtl = (CEngCtl*)arg;
    int sockfd = -1;

    /* creat socket server */
    if (-1 == create_socket_local_server(lpEngCtl)) {
        EngLog::error("eng_soc thread start error");
        return NULL;
    }

    while (1) {
        EngLog::info("%s eng_connect_fd=%d", __FUNCTION__, sockfd);
        if ((sockfd = ::accept(lpEngCtl->m_fdSockSrv, (struct sockaddr *)NULL, NULL)) == -1) {
            EngLog::error("accept socket error: %s(errno: %d)", strerror(errno), errno);
            continue;
        }

        EngLog::info("%s connected! eng_connect_fd:%d socket_name=%s", __FUNCTION__, sockfd, SOCK_ENGCTL_NAME);
        eng_socket_rw_thread(&sockfd, (void* )lpEngCtl);
    }

    out:
        ::close(lpEngCtl->m_fdSockSrv);
        lpEngCtl->m_fdSockSrv = -1;

    return NULL;
}

int CEngCtl::create_socket_local_server(CEngCtl* lpEngCtl) {
    lpEngCtl->m_fdSockSrv = socket_local_server(SOCK_ENGCTL_NAME, ANDROID_SOCKET_NAMESPACE_ABSTRACT, SOCK_STREAM);
    if (lpEngCtl->m_fdSockSrv < 0) {
        EngLog::error("open %s failed: %s\n", SOCK_ENGCTL_NAME, strerror(errno));
        return -1;
    }
    if (::listen(lpEngCtl->m_fdSockSrv, 4) < 0) {
        EngLog::error("listen %s failed: %s\n", SOCK_ENGCTL_NAME, strerror(errno));
        ::close(lpEngCtl->m_fdSockSrv);
        return -1;
    }
    return 0;
}

void CEngCtl::eng_socket_rw_thread(void *fd, void *arg)
{
    int soc_fd;
    int ret;
    int length;
    int type;
    fd_set readfds;
    CEngCtl* lpEngCtl = (CEngCtl*)arg;

    soc_fd = *(int *)fd;
    EngLog::info("%s soc_fd = %d ", __FUNCTION__, soc_fd);
    while(1) {
        FD_ZERO(&readfds);
        FD_SET(soc_fd,&readfds);
        ret = ::select(soc_fd+1,&readfds,NULL,NULL,NULL);
        if (ret < 0) {
            EngLog::error("%s  ret = %d, break",__FUNCTION__, ret);
            break;
        }
        memset((void*)&(lpEngCtl->m_cmd_recv),0,sizeof(lpEngCtl->m_cmd_recv));
        memset(lpEngCtl->m_cmd_rsp,0,sizeof(lpEngCtl->m_cmd_rsp));
        if (FD_ISSET(soc_fd,&readfds)) {
            length = ::read(soc_fd,(char*)&(lpEngCtl->m_cmd_recv),sizeof(lpEngCtl->m_cmd_recv));
            if (length <= 0) {
                EngLog::error("%s length = %d, break",__FUNCTION__, length);
                break;
            }
            EngLog::info("%s m_cmd_recv = %d",__FUNCTION__, length);
            ret = process(&(lpEngCtl->m_cmd_recv), lpEngCtl->m_cmd_rsp, sizeof(lpEngCtl->m_cmd_rsp));
            EngLog::info("%s process return %d: %s", __FUNCTION__, ret, lpEngCtl->m_cmd_rsp);
            ret = ::write(soc_fd,lpEngCtl->m_cmd_rsp,sizeof(lpEngCtl->m_cmd_rsp));
            EngLog::info("%s write return %d", __FUNCTION__, ret);
        }
    }
    EngLog::info("%s CLOSE_SOCKET",__FUNCTION__);
    ::close(soc_fd);
}

/*
defined in engpcctl.h

typedef enum{
    ENGPC_DST_DEV,
    ENGPC_DST_CHNL,

    ENGPC_DST_NONE,
}ENGPC_DST;

typedef enum{
    ENGPC_ACT_LIST,
    ENGPC_ACT_ADD,
    ENGPC_ACT_DEL,
    ENGPC_ACT_ENABLE,
    ENGPC_ACT_DISABLE,

    ENGPC_ACT_NONE,
}ENGPC_ACT;
*/

int CEngCtl::process(LPENGPC_CMD_T lpCmd, char*rsp, int len){
    if (lpCmd == NULL){
        return -1;
    }

    printfCmd(lpCmd);

    if(lpCmd->dst == 0){
       return ctl_dev(lpCmd, rsp, len);
    }else if(lpCmd->dst == 1){
       return ctl_chnl(lpCmd, rsp, len);
    }else{
    }

    return 0;
}

int CEngCtl::ctl_dev(LPENGPC_CMD_T lpCmd, char*rsp, int len){
    int ret = -1;
    //0: list, 1: add, 2: del, 3: enable, 4:disable
    switch (lpCmd->act){
    case 0:
        break;
    case 1:
        break;
    case 2:
        break;
    case 3:
        break;
    case 4:{
        }
        break;
    default:
        break;
    }

    return -1;
}

int CEngCtl::ctl_chnl(LPENGPC_CMD_T lpCmd, char*rsp, int len){
    int ret = -1;
    //0: list, 1: add, 2: del, 3: enable, 4:disable
    switch (lpCmd->act){
    case 0:
        break;
    case 1:
        break;
    case 2:
        break;
    case 3:
    case 4:{
            if (m_lpChnlMgr != NULL){
                CChnlThreadMgr* lpChnlThreadMgr = m_lpChnlMgr->getChnlThreadMgr();
                if(lpChnlThreadMgr != NULL){
                    lpChnlThreadMgr->enableChnl(lpCmd->chnl_s.name, lpCmd->act==3?true:false);
                    return 0;
                }
            }
        }
        break;
    default:
        break;
    }

    return -1;
}

void CEngCtl::printfCmd(LPENGPC_CMD_T lpCmd){
    if (lpCmd != NULL){
        EngLog::info("cmd: ");
        EngLog::info("    dst = %d ", lpCmd->dst);
        EngLog::info("    act = %d ", lpCmd->act);
        if (lpCmd->dst == 0){
            EngLog::info("    devName = %s ", lpCmd->dev_s.devName);
            EngLog::info("    bootmode = %s ", lpCmd->dev_s.bootmode);
            EngLog::info("    portName = %s ", lpCmd->dev_s.portName);
            EngLog::info("    portType = %s ", lpCmd->dev_s.portType);
            EngLog::info("    portPath = %s ", lpCmd->dev_s.portPath);
            EngLog::info("    dataType = %s ", lpCmd->dev_s.dataType);
        }else if(lpCmd->dst == 1){
            EngLog::info("    bootmode = %s ", lpCmd->chnl_s.bootmode);
            EngLog::info("    name = %s ", lpCmd->chnl_s.name);
            EngLog::info("    srcDevName = %s ", lpCmd->chnl_s.srcDevName);
            EngLog::info("    srcPortName = %s ", lpCmd->chnl_s.srcPortName);
            EngLog::info("    dstDevName = %s ", lpCmd->chnl_s.dstDevName);
            EngLog::info("    dstPortName = %s ", lpCmd->chnl_s.dstPortName);
            EngLog::info("    act = %s ", lpCmd->chnl_s.act);
            EngLog::info("    dataType = %s ", lpCmd->chnl_s.dataType);
            EngLog::info("    enable = %s ", lpCmd->chnl_s.enable);
            EngLog::info("    ap_process = %s ", lpCmd->chnl_s.ap_process);
        }else{
        }
    }
}
