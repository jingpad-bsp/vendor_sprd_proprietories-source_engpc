#ifndef __CMODULE_MANAGER_H__
#define __CMODULE_MANAGER_H__


#include "modules.h"
#include "dev.h"
#include "sprd_fts_type.h"
#include "CPort.h"

#define MAX_MOUDLUE_PATH  1024
#define MAX_MODULE_FILE_PATH  MAX_MOUDLUE_PATH

typedef void (*REGISTER_FUNC)(struct eng_callback *register_callback);
typedef void (*REGISTER_EXT_FUNC)(struct eng_callback *reg, int *num);

class CModuleMgr{
    public:
        CPort* m_lpHostDiagPort;
        CPort* m_lpHostWcnATPort;

        virtual ~CModuleMgr();

        static CModuleMgr* getInstance(char* dir){
            return new CModuleMgr(dir);
        }

        const char* TAG(){return "CModuleMgr";}

        int load();
        bool isLoaded() {return m_isLoaded;}

        int process(DATA_TYPE type, char *buf, int len, char *rsp, int rsp_len, int& cp_process);

        int processDiag(DATA_TYPE type, char *buf, int len, char *rsp, int rsp_len, int& cp_process);
        int processAT(DATA_TYPE type, char *buf, int len, char *rsp, int rsp_len, int& cp_process);
        int processSmp(DATA_TYPE type, char *buf, int len, char *rsp, int rsp_len, int& cp_process);
        int processWcnAT(DATA_TYPE type, char *buf, int len, char *rsp, int rsp_len, int& cp_process);

        void internalRegCallBack(struct eng_callback *p, int num);

        void regDiagHost(CPort *lpPort){m_lpHostDiagPort = lpPort;}
        void regWcnATHost(CPort *lpPort){m_lpHostWcnATPort = lpPort;}
        void setPendingMark(){m_lpPendingCB = m_lpCurCB;}
        void clearPendingMark(){m_lpPendingCB = NULL;}
        void print();

    private:
        char m_path[MAX_MOUDLUE_PATH];
        struct list_head m_listHead;
        bool m_isLoaded;
        eng_modules *m_lpCurCB;

        CModuleMgr(char* dir);
        eng_modules* get_eng_modules(struct eng_callback p);
        int eng_modules_load();
        eng_modules* m_lpPendingCB;

        static void* m_libHdl;
        static void* m_libInterface;
    public:
        int bqb_vendor_open();
};

#endif