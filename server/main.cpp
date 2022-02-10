#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <pthread.h>
#include <cutils/android_filesystem_config.h>

#include "englog.h"
#include "CDevMgr.h"
#include "CChnlThreadMgr.h"
#include "CChnlMgr.h"
#include "CModuleMgr.h"
#include "bootmode.h"
#include "adapter.h"
#include "cpctl.h"
#include "engctl.h"

#define CHNL_DEV_DIR_PATH "/vendor/etc/engpc/dev"
#define CHNL_CHNL_DIR_PATH "/vendor/etc/engpc/chnl"
#define DYNAMIC_SO_DIR_PATH "/vendor/lib/npidevice"

#define SAFE_DELETE(s) {if (s != NULL) delete s;}

int wait_for_modem_alive(int timeout);
void wait_for_data_ready();
void switchGidToRoot(void);

CModuleMgr* g_lpModMgr = NULL;
CDevMgr* g_lpDevMgr = NULL;

void internalRegCallback(){
    EngLog::regCallBack(g_lpModMgr);
    CCPCtl::regCallBack(g_lpModMgr);
}

int main(){

    EngLog::info("main...");

    const char* bootmode = initBootMode();
    EngLog::info("bootmode = %s", bootmode);

    if (strcasecmp(bootmode, BOOTMODE_CALI) == 0){
        switchGidToRoot();
    }

    //init module manager
    EngLog::info("load dynamic so");
    g_lpModMgr = CModuleMgr::getInstance(DYNAMIC_SO_DIR_PATH);
    if (NULL == g_lpModMgr /*|| 0 != g_lpModMgr->load()*/){ // load so when pc send diag cmd first
        EngLog::error("load dynamic so fail");
    }

    //init device manager
    EngLog::info("load dev manager...");
    g_lpDevMgr = CDevMgr::getInstance();
    if (NULL == g_lpDevMgr || 0 != g_lpDevMgr->load((char*)CHNL_DEV_DIR_PATH)){
        EngLog::error("load devmgr fail.");
        SAFE_DELETE(g_lpModMgr);
        return -1;
    }
    g_lpDevMgr->print();

    //init thread manager
    EngLog::info("load thread manager...");
    CChnlThreadMgr* lpThreadMgr = CChnlThreadMgr::getInstance();
    if (NULL == lpThreadMgr){
        EngLog::error("init threadmgr fail.");
        SAFE_DELETE(g_lpModMgr);
        SAFE_DELETE(g_lpDevMgr);
        return -1;
    }

    //init channle manager
    EngLog::info("load chnl manager...");
    CChnlMgr* lpChnlMgr = CChnlMgr::getInstance(lpThreadMgr, g_lpDevMgr, g_lpModMgr);
    if (NULL == lpChnlMgr || 0 != lpChnlMgr->load((char*)CHNL_CHNL_DIR_PATH)){
        EngLog::error("init chnlmgr fail.");
        SAFE_DELETE(g_lpModMgr);
        SAFE_DELETE(g_lpDevMgr);
        SAFE_DELETE(lpThreadMgr);
        return -1;
    }
    lpChnlMgr->print();

    //enable channel && device
    EngLog::info("enable %s", bootmode);
    lpChnlMgr->enable(bootmode, true);
    EngLog::info("active mode");
    g_lpDevMgr->activeMode((char* )bootmode);

    // register diag port&wcn at port
    EngLog::info("set host dev diag port");
    CDev* lpDev = g_lpDevMgr->find(DEV_HOST_NAME);
    if (lpDev != NULL){
        g_lpModMgr->regDiagHost(lpDev->find(DEV_HOST_DIAG_NAME));
    }else{
        EngLog::info("can not find host dev diag port.");
    }
    EngLog::info("set host dev wcn at port");
    lpDev = g_lpDevMgr->find(DEV_HOST_NAME);
    if (lpDev != NULL){
        g_lpModMgr->regWcnATHost(lpDev->find(DEV_HOST_WCN_AT_NAME));
    }else{
        EngLog::info("can not find host wcn at port.");
    }

#ifndef ENGPC_AP_CALI
    //wait for modem alive
    EngLog::info("wait for modem alive");
    if (0 != wait_for_modem_alive(600)){
        EngLog::error("wait modem alive fail!!!!!!!!! please check modem status!");
    }
#else
    //if ap cali only, so no need to wait modem alive
#endif

    //wait for data section ready
    if (strcasecmp(bootmode, BOOTMODE_NORMAL) == 0
        || strcasecmp(bootmode, BOOTMODE_NORMAL_LITE) == 0
        || strcasecmp(bootmode, BOOTMODE_AUTOTEST) == 0){
        //wait_for_data_ready();
    }

    //set usb work mode: vser or gser
    usb_mode(bootmode);
    //monitor usb plug-in or plug-out
    usb_monitor(CDevMgr::notify);

    //cp log ctrl monitor && cp ap time sync
    EngLog::info("cp log ctrl, cp ap time sync");
    CCPCtl* lpCpCtl = new CCPCtl();
    lpCpCtl->attach(lpChnlMgr);
    lpCpCtl->run();
    //engpc ctl srv
    EngLog::info("engpc ctrl");
    CEngCtl* lpEngCtl = new CEngCtl();
    lpEngCtl->attach(lpChnlMgr);
    lpEngCtl->run();

    //is modem to pc? is wcn to pc?
    if (strcasecmp(bootmode, BOOTMODE_NORMAL) == 0 || strcasecmp(bootmode, BOOTMODE_NORMAL_LITE) == 0){
        char logdest[8] = {0};
        char logtype[8] = {0};
        logtype[0] = LOG_TYPE_NAME_MODEM;
        sys_getlogdest(logtype, logdest);
        EngLog::info("modem log dest: %s", logdest);
        if (logdest[0] != LOG_LOCATION_PC){
            lpChnlMgr->resetWithDevName(DEV_MODEM_NAME, false);
        }
        logtype[0] = LOG_TYPE_NAME_WCN;
        sys_getlogdest(logtype, logdest);
        EngLog::info("wcn log dest: %s", logdest);
        if (logdest[0] == LOG_LOCATION_PC){
            lpChnlMgr->resetWithDevName(DEV_WCN_NAME, true);
            g_lpModMgr->bqb_vendor_open();
        }else{
            lpChnlMgr->resetWithDevName(DEV_WCN_NAME, false);
        }
    }

    //internal reg callback
    internalRegCallback();

    //work here
    EngLog::info("run...");
    lpChnlMgr->run(bootmode);

    while(1){
        sleep((unsigned int)(-1));
    }

    return 1;
}

void wait_for_data_ready(){
    int count = 60;
    while(!is_data_dir_ready() && count-- > 0){
        sleep(1);
    }
    if (count <= 0){
        EngLog::error("warning!!!!!!!!!!wait for encrypt fail.");
    }
}

int wait_for_modem_alive(int timeout)
{
    int fd = -1;
    int try_cnt = timeout;

    EngLog::info("timeout = %d", timeout);

    CDev* lpDev = g_lpDevMgr->find(DEV_MODEM_NAME);
    if (lpDev != NULL){
        CPort* lpPort = lpDev->find(DEV_MODEM_DIAG_NAME);
        if (lpPort != NULL){
            char* path = (char* )(lpPort->getpath());
            EngLog::info("path = %s", path);
            do {
                fd = open(path, O_RDWR | O_NONBLOCK);
                if (fd >= 0) {
                    break;
                }
                if (try_cnt%10 == 0){
                    EngLog::info("%s: times:%d, failed to open tty dev:  %s, fd = %d",__FUNCTION__, try_cnt, path, fd);
                }
                usleep(100 * 1000);
                try_cnt--;
            } while (try_cnt > 0);
            if (fd >= 0){
                EngLog::info("%s: times:%d, succ open tty dev:  %s, fd = %d",__FUNCTION__, try_cnt, path, fd);
                close(fd);
                return 0;
            }else{
                return -1;
            }
        }else{
            EngLog::error("can't find port: COM_DIAG");
        }
    }else{
        EngLog::error("can't find dev: cp");
    }

    return 0;
}

void switchGidToRoot(void){
    gid_t list[64];
    gid_t gid;
    int n, max;

    EngLog::info("%s", __FUNCTION__);
    max = getgroups(64, list);
    gid = getgid();

    if (max < 0) max = 0;
    list[max++] = gid;

    if (setgroups(max, list)) {
        EngLog::error("setgroups failed: %s", strerror(errno));
    }
    if (setgid(AID_ROOT)){
        EngLog::error("set failed: %s", strerror(errno));
    }
}