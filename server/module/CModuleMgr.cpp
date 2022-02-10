#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <dlfcn.h>
#include <sys/ioctl.h>
#include <sys/types.h>

#include "CModuleMgr.h"
#include "englog.h"
#include "modules.h"
#include "bqb.h"
#include "CProtolDiag.h"


extern CModuleMgr* g_lpModMgr;

#define MAX_CHNL_BUFF  (64*1024)

int write_to_host_diag(char* buff, int len){
    if (g_lpModMgr != NULL && g_lpModMgr->m_lpHostDiagPort != NULL){
        char *dst = (char* )malloc(len*2);
        if (dst == NULL){
            EngLog::error("malloc %d fail", len*2);
            return 0;
        }
        memset(dst, 0, len*2);
        int retlen = CProtolDiag::translate_packet(dst, buff+1, len-2);
        retlen = g_lpModMgr->m_lpHostDiagPort->write(dst, retlen);
        free(dst);
        return retlen;
    }else{
        EngLog::error("write_to_host_diag fail: g_lpModMgr = NULL or s_lpModMgr->m_lpHostDiagPort = NULL");
    }

    return 0;
}

int chnl_send_diag_interface(char* buff, int len){
    char rsp[MAX_CHNL_BUFF] = {0};
    int cp_process = 0;
    if (g_lpModMgr != NULL){
        return g_lpModMgr->process(DATA_DIAG, buff, len, rsp, sizeof(rsp), cp_process);
    }

    return 0;
}

int chnl_send_at_interface(char* buff, int len){
    char rsp[MAX_CHNL_BUFF] = {0};
    int cp_process = 0;
    if (g_lpModMgr != NULL){
        return g_lpModMgr->process(DATA_AT, buff, len, rsp, sizeof(rsp), cp_process);
    }

    return 0;
}

DYMIC_WRITETOPC_FUNC write_interface[WRITE_TO_MAX] = {
    write_to_host_diag,/*pc lte diag*/
    NULL,NULL,NULL,NULL,NULL,
    chnl_send_diag_interface,
    NULL,
    chnl_send_at_interface,/*send cmd so to so*/
    NULL,NULL,NULL,NULL,NULL,NULL
};

static const char *VENDOR_LIBRARY_NAME = "libbqbbt.so";
static const char *VENDOR_LIBRARY_SYMBOL_NAME = "BLUETOOTH_BQB_INTERFACE";

void* CModuleMgr::m_libHdl = NULL;
void* CModuleMgr::m_libInterface = NULL;

CModuleMgr::CModuleMgr(char* dir){
    memcpy(m_path, dir, strlen(dir));
    m_lpHostDiagPort = NULL;
    m_lpHostWcnATPort = NULL;
    m_isLoaded = false;
    m_lpPendingCB = NULL;
    m_lpCurCB = NULL;
    INIT_LIST_HEAD(&m_listHead);
}

CModuleMgr::~CModuleMgr(){
}

int CModuleMgr::load(){
    int ret = eng_modules_load();
    m_isLoaded = (ret==0)?true:false;
    return ret;
}

int CModuleMgr::process(DATA_TYPE type, char *buf, int len, char *rsp, int rsp_len, int& cp_process){
    if (!m_isLoaded){
        load();
    }

    if (type == DATA_AT)
        return processAT(type, buf, len, rsp, rsp_len, cp_process);
    if (type == DATA_WCN_AT)
        return processWcnAT(type, buf, len, rsp, rsp_len, cp_process);
    if (type == DATA_DIAG)
        return processDiag(type, buf, len, rsp, rsp_len, cp_process);
    if (type == DATA_AGDSP_LOG||type == DATA_AGDSP_MEM||type == DATA_AGDSP_PCM)
        return processSmp(type, buf, len, rsp, rsp_len, cp_process);

    return 0;
}

int CModuleMgr::processDiag(DATA_TYPE type, char *buf, int len, char *rsp, int rsp_len, int& cp_process){
    eng_modules *modules_list = NULL;
    struct list_head *list_find = NULL;
    MSG_HEAD_T *msg_head_ptr = (MSG_HEAD_T *)(buf + 1);
    int rlen = 0;
    unsigned int *data_cmd = NULL;
    byte *data = NULL;
    unsigned short *apcmd = NULL;
    int bProcess= 0;

    list_for_each(list_find,&m_listHead){
        modules_list = list_entry(list_find, eng_modules, node);
        m_lpCurCB = modules_list;

        if ((buf[7] == 0x68) && (0 != strlen(modules_list->callback.at_cmd)) && (strcasestr(buf+9, modules_list->callback.at_cmd)) != NULL) { // at command
            EngLog::debug("%s: Dymic CMD=%s finded\n",__FUNCTION__,modules_list->callback.at_cmd);
            if (NULL != modules_list->callback.eng_linuxcmd_func) {
                rlen = modules_list->callback.eng_linuxcmd_func(buf, rsp);

                //if lib*.so return not support, then continue
                if (rlen == ENG_DIAG_RET_UNSUPPORT){
                    EngLog::debug("%s eng_diag_func return %d, continue find next callback func", __FUNCTION__, ENG_DIAG_RET_UNSUPPORT);
                    continue;
                }
                //for case :need to ap & cp
                if (modules_list->callback.also_need_to_cp) {
                    cp_process = 1;
                }

                bProcess = 1;
                break;
            } else {
                EngLog::debug("%s: Dymic eng_linuxcmd_func == NULL\n",__FUNCTION__);
                break;
            }
        } else if ( (msg_head_ptr->type == modules_list->callback.type && msg_head_ptr->subtype == modules_list->callback.subtype) ||
                    (modules_list->callback.eng_cmd_match != NULL && modules_list->callback.eng_cmd_match(buf, len) == 0) )  {
            EngLog::debug("%s: type = %d, subtype = %d", __FUNCTION__, msg_head_ptr->type, msg_head_ptr->subtype);
            // diag command: type(unsigned char) + sub_type(unsigned char) + data_cmd(unsigned int)
            if (0x5D == msg_head_ptr->type){
                data_cmd = (unsigned int *)(buf + 1 + sizeof(MSG_HEAD_T));
                if (*data_cmd != modules_list->callback.diag_ap_cmd) {
                    EngLog::debug("%s data cmd is not matched!", __FUNCTION__);
                    continue;
                }
            }

            // diag command: type(unsigned char) + sub_type(unsigned char) + TOOLS_DIAG_AP_CMD_T
            if (0x62 == msg_head_ptr->type){//DIAG_CMD_APCALI
                apcmd = (unsigned short *)(buf + 1 + sizeof(MSG_HEAD_T));
                if (*apcmd != modules_list->callback.diag_ap_cmd) {
                    EngLog::debug("%s apcmd->cmd is not matched!", __FUNCTION__);
                    continue;
                }
            }

            // diag command: type(unsigned char) + sub_type(unsigned char) + byte, for autotest test case which have same type+subtype
            if (0x38 == msg_head_ptr->type){
                data = (byte *)(buf + 1 + sizeof(MSG_HEAD_T));
                if (modules_list->callback.diag_ap_cmd != -1 && (int)(*data) != modules_list->callback.diag_ap_cmd)  {
                    EngLog::debug("%s data is not matched!", __FUNCTION__);
                    continue;
                }
            }

            if (NULL != modules_list->callback.eng_diag_func) {
                if (NULL != modules_list->callback.eng_set_writeinterface_func) {
                    modules_list->callback.eng_set_writeinterface_func(write_interface);
                }
                rlen = modules_list->callback.eng_diag_func(buf, len, rsp, rsp_len);
                if (rlen == ENG_DIAG_RET_UNSUPPORT){
                    EngLog::debug("%s eng_diag_func return %d, continue find next callback func", __FUNCTION__, ENG_DIAG_RET_UNSUPPORT);
                    continue;
                }

                //for case :need to ap & cp
                if (modules_list->callback.also_need_to_cp) {
                    cp_process = 1;
                }

                bProcess =1;
                break;
            } else {
                EngLog::debug("%s: Dymic eng_diag_func == NULL\n",__FUNCTION__);
                break;
            }
        } else {
            continue;
        }
    }

    //process pending at cmd
    if (bProcess == 0 && m_lpPendingCB != NULL && 0x68 == msg_head_ptr->type && strcasestr(buf + 9, "AT") == NULL){
        EngLog::debug("%s: Dymic next_data_callback", __FUNCTION__);
        rlen = m_lpPendingCB->callback.eng_linuxcmd_func(buf, rsp);
    }else{
        m_lpPendingCB = NULL;
    }

    return rlen;
}

int CModuleMgr::processAT(DATA_TYPE type, char *buf, int len, char *rsp, int rsp_len, int& cp_process){
    int rlen = 0, ret = 0;
    eng_modules *modules_list = NULL;
    struct list_head *list_find;

    EngLog::info("%s: buf:%s len:%d rsp_len:%d", __FUNCTION__, buf, len, rsp_len);

    list_for_each(list_find, &m_listHead) {
        modules_list = list_entry(list_find, eng_modules, node);

        if ((0 != strlen(modules_list->callback.at_cmd)) &&
            (0 == strncasecmp((const char *) buf, (const char *)(modules_list->callback.at_cmd),
            strlen(modules_list->callback.at_cmd)))) { // at command
            EngLog::info("%s: Dymic CMD=%s finded\n", __FUNCTION__, modules_list->callback.at_cmd);
            if (NULL != modules_list->callback.eng_linuxcmd_func) {
                rlen = modules_list->callback.eng_linuxcmd_func(buf, rsp);

                //if lib*.so return not support, then continue
                if (rlen == ENG_DIAG_RET_UNSUPPORT){
                    EngLog::info("%s: eng_at_dymic_hdlr return ENG_DIAG_RET_UNSUPPORT, continue", __FUNCTION__);
                    continue;
                }
                //for case :need to ap & cp
                if (modules_list->callback.also_need_to_cp) {
                    cp_process = 1;
                }

                break;
            } else {
              EngLog::info("%s: Dymic eng_linuxcmd_func == NULL\n", __FUNCTION__);
              break;
            }
        }else {
          continue;
        }
    }

    return rlen;
}

int CModuleMgr::processSmp(DATA_TYPE type, char *buf, int len, char *rsp, int rsp_len, int& cp_process){
    memcpy(rsp, buf, len);
    return len;
}

int CModuleMgr::processWcnAT(DATA_TYPE type, char *buf, int len, char *rsp, int rsp_len, int& cp_process){
    int rlen = 0;

    rlen = processAT(type, buf, len, rsp, rsp_len, cp_process);
    if (rlen > 0 || strlen(rsp) > 0){
        return rlen;
    }

    if (m_libHdl == NULL){
        bqb_vendor_open();
    }

    if(m_libInterface != NULL ){
        bt_bqb_interface_t* pIf = (bt_bqb_interface_t *)m_libInterface;
        if (m_lpHostWcnATPort != NULL){
            pIf->set_fd(m_lpHostWcnATPort->getFD());
            rlen = pIf->check_received_str(m_lpHostWcnATPort->getFD(), buf, len);
        }

        if (0 == rlen && pIf->get_bqb_state() == BQB_OPENED){
            pIf->eng_send_data(buf, len);
        }
    }else{
        EngLog::error("m_libInterface is NULL!");
    }

    return ENG_DIAG_NO_RESPONSE;
}

int CModuleMgr::bqb_vendor_open(){
    m_libHdl = dlopen(VENDOR_LIBRARY_NAME, RTLD_NOW);
    if (!m_libHdl) {
        EngLog::error("unable to open %s: %s", VENDOR_LIBRARY_NAME, dlerror());
        goto error;
    }

    m_libInterface = (bt_bqb_interface_t *)dlsym(m_libHdl, VENDOR_LIBRARY_SYMBOL_NAME);
    if (!m_libInterface) {
        EngLog::error("unable to find symbol %s in %s: %s", VENDOR_LIBRARY_SYMBOL_NAME, VENDOR_LIBRARY_NAME, dlerror());
        goto error;
    }
    ((bt_bqb_interface_t *)m_libInterface)->init();

    return 0;

error:
    m_libInterface = NULL;
    if (m_libHdl)
    dlclose(m_libHdl);
    m_libHdl = NULL;

    return -1;
}

eng_modules* CModuleMgr::get_eng_modules(struct eng_callback p){
    eng_modules *modules = (eng_modules*)malloc(sizeof(eng_modules));
    if (modules == NULL)
    {
        EngLog::error("%s malloc fail...",__FUNCTION__);
        return NULL;
    }
    memset(modules,0,sizeof(eng_modules));
    modules->callback.type = p.type;
    modules->callback.subtype = p.subtype;
    modules->callback.diag_ap_cmd = p.diag_ap_cmd;
    modules->callback.also_need_to_cp = p.also_need_to_cp;
    sprintf(modules->callback.at_cmd, "%s", p.at_cmd);
    modules->callback.eng_diag_func = p.eng_diag_func;
    modules->callback.eng_linuxcmd_func = p.eng_linuxcmd_func;
    modules->callback.eng_set_writeinterface_func = p.eng_set_writeinterface_func;
    modules->callback.eng_cmd_match = p.eng_cmd_match;

    return modules;
}

int CModuleMgr::eng_modules_load(){
    REGISTER_FUNC eng_register_func = NULL;
    REGISTER_EXT_FUNC eng_register_ext_func = NULL;
    struct eng_callback register_callback;
    struct eng_callback register_arr[64];
    struct eng_callback *register_arr_ptr = register_arr;
    int register_num = 0;
    int i = 0;
    char path[MAX_MODULE_FILE_PATH]=" ";
    char lnk_path[MAX_MODULE_FILE_PATH]=" ";
    int readsize = 0;

    //get so name fail:empty
    DIR *dir;
    struct dirent *ptr;
    void *handler = NULL;

    EngLog::info("%s",__FUNCTION__);

    if ((dir = opendir(m_path)) == NULL)
    {
        EngLog::error("Open %s error...%s",m_path,dlerror());
        return 0;
    }

    while ((ptr = readdir(dir)) != NULL)
    {
        if (ptr->d_type == 8 || ptr->d_type == 10) { /// file  , 10 == DT_LNK
            EngLog::info("d_name:%s/%s", m_path, ptr->d_name);
            snprintf(path, sizeof(path), "%s/%s", m_path, ptr->d_name);
            EngLog::info("find lib path: %s", path);

            if (ptr->d_type == 10) //DT_LNK
            {
                memset(lnk_path,0,sizeof(lnk_path));
                readsize = readlink(path, lnk_path, sizeof(lnk_path));
                EngLog::info("%s readsize:%d lnk_path:%s \n", path ,readsize, lnk_path);

                if(readsize == -1) {
                    EngLog::error("ERROR! Fail to readlink!\n");
                    continue;
                }

                memset(path, 0, sizeof(path));
                strncpy(path, lnk_path, strlen(lnk_path));
            }

            if (access(path, R_OK) == 0) {
                handler = dlopen(path, RTLD_LAZY);
                if (handler == NULL) {
                    EngLog::error("%s dlopen fail! %s \n", path, dlerror());
                } else {
                    eng_register_func = (REGISTER_FUNC)dlsym(handler, "register_this_module");
                    if (eng_register_func != NULL) {
                        eng_modules *modules = NULL;
                        memset(&register_callback, 0, sizeof(struct eng_callback));
                        register_callback.diag_ap_cmd = -1;
                        register_callback.type = 0xFF;
                        register_callback.subtype = 0xFF;
                        eng_register_func(&register_callback);
                        EngLog::info("%d:type:%d subtype:%d data_cmd:%d at_cmd:%s", i,
                        register_callback.type, register_callback.subtype,
                        register_callback.diag_ap_cmd, register_callback.at_cmd);
                        modules = get_eng_modules(register_callback);
                        if (modules == NULL) {
                            EngLog::error("%s modules == NULL\n", __FUNCTION__);
                            continue;
                        }
                        list_add_tail(&modules->node, &m_listHead);
                    }

                    eng_register_ext_func = (REGISTER_EXT_FUNC)dlsym(handler, "register_this_module_ext");
                    if (eng_register_ext_func != NULL) {
                        eng_modules *modules = NULL;
                        memset(register_arr, 0, sizeof(register_arr));
                        for (i = 0; i < sizeof(register_arr)/sizeof(struct eng_callback); i++) {
                            register_arr[i].diag_ap_cmd = -1;
                            register_arr[i].type = 0xFF;
                            register_arr[i].subtype = 0xFF;
                        }
                        eng_register_ext_func(register_arr_ptr, &register_num);
                        EngLog::info("register_num:%d",register_num);

                        for (i = 0; i < register_num; i++) {
                            EngLog::info("%d:type:%d subtype:%d data_cmd:%d at_cmd:%s", i,
                            register_arr[i].type, register_arr[i].subtype,
                            register_arr[i].diag_ap_cmd, register_arr[i].at_cmd);
                            modules = get_eng_modules(register_arr[i]);
                            if (modules == NULL) {
                                EngLog::error("%s modules == NULL\n", __FUNCTION__);
                                continue;
                            }
                            list_add_tail(&modules->node, &m_listHead);
                        }
                    }
                    if (eng_register_func == NULL && eng_register_ext_func == NULL) {
                        dlclose(handler);
                        EngLog::error("%s dlsym fail! %s\n", path, dlerror());
                        continue;
                    }
                }
            } else {
                EngLog::error("%s is not allow to read!\n", path);
            }
        }
    }

    closedir(dir);
    return 0;
}

void CModuleMgr::internalRegCallBack(struct eng_callback *p, int num){
    for (int i = 0; i < num; i++){
        EngLog::info("%d:type:%d subtype:%d data_cmd:%d at_cmd:%s", i,
                     p[i].type, p[i].subtype, p[i].diag_ap_cmd, p[i].at_cmd);

        eng_modules *modules = get_eng_modules(p[i]);
        if (modules == NULL) {
            EngLog::error("%s modules == NULL\n", __FUNCTION__);
            continue;
        }
        list_add_tail(&modules->node, &m_listHead);
    }
}

void CModuleMgr::print(){
/*
    eng_modules *modules_list = NULL;
    struct list_head *list_find = NULL;

    list_for_each(list_find,&m_listHead){
        modules_list = list_entry(list_find, eng_modules, node);

    }
*/
}
