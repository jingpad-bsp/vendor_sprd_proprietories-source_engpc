#ifndef __ENGPC_ENGCTL_H__
#define __ENGPC_ENGCTL_H__

#include "CChnlMgr.h"
#include "eng_ctl.h"

#define CMD_RSP_LEN 256

class CEngCtl {
    public:
        CEngCtl();
        ~CEngCtl();

        void attach(CChnlMgr* lpChnlMgr);
        void run();

        static int process(LPENGPC_CMD_T lpCmd, char*rsp, int len);

    private:
        int m_fdSockSrv;
        static CChnlMgr* m_lpChnlMgr;

        ENGPC_CMD_T m_cmd_recv;
        char m_cmd_rsp[CMD_RSP_LEN];

        static void* engCtl(void *arg);
        static int create_socket_local_server(CEngCtl* lpEngCtl);
        static void eng_socket_rw_thread(void *fd, void *arg);
        static int ctl_dev(LPENGPC_CMD_T lpCmd, char*rsp, int len);
        static int ctl_chnl(LPENGPC_CMD_T lpCmd, char*rsp, int len);
        static void printfCmd(LPENGPC_CMD_T lpCmd);
};


#endif