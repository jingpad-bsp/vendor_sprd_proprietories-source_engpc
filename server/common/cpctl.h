#ifndef __ENGPC_CPLOGCTL_H__
#define __ENGPC_CPLOGCTL_H__

#include <pthread.h>
#include "CChnlMgr.h"
#include "CModuleMgr.h"

//cp log ctrl
#define SOCK_CPLOGCTL_NAME "engpc_soc.l"
#define ENG_DATA_LENGTH 3

typedef struct{
    char sn;
    char log_type;
    union{
        char location;
        char result;
    };
}ENG_DATA_T;

//time sync
#define FDS_SIZE 2
enum CPEVENT_E {
  CE_ALIVE,
  CE_ASSERT,
  CE_RESET,
  CE_BLOCKED,
  CE_NONE
};

enum SUBSYS_E {
  SS_ALL, // all system, or more sub system
  SS_MODEM, // 3G/4G MOOEM
  SS_WCN, // WCN
  SS_GNSS, // GNSS
  SS_PM_SH // PM and Sensor Hub
};

typedef struct modem_timestamp
{
  uint32_t tv_sec;             /* clock time, seconds since 1970.01.01 */
  uint32_t tv_usec;            /* clock time, microeseconds part */
  uint32_t sys_cnt;            /* modem's time */
}__attribute__((packed))MODEM_TIMESTAMP_T;

typedef struct time_sync
{
  uint32_t sys_cnt;
  uint32_t uptime;
}__attribute__((packed))TIME_SYNC_T;

struct SocketConnection {
  int fd;
  int index; // Index in the fds array
  int max_retry; // Max number of connection trials, 0 for no limit
  int try_num; // Number of unsuccessful connections
};

typedef struct cp_event_notify
{
  uint8_t subsys; // 0,事件涉及整个手机系统或多个子系统;1,3G/4G MOOEM;2,WCN;3,GNSS;4,PM和Sensor Hub;>4,reserve;
  uint8_t event; // 0,启动成功;1,发生assert;2,发生重启;3,发生BLOCK;>3,reserve;
}__attribute__((packed))CP_EVENT_NOTIFY_T;

#define TIME_SERVER_SOCK_NAME "cp_time_sync_server"
#define MODEM_SOCKET_NAME "modemd"

#define MAXLENRESP 10
#define SLOG_MODEM_SERVER_SOCK_NAME "slogmodem"
#define ENABLE_WCN_LOG_CMD  "ENABLE_LOG WCN\n"
#define DISABLE_WCN_LOG_CMD  "DISABLE_LOG WCN\n"
#define ENABLE_GNSS_LOG_CMD  "ENABLE_LOG GNSS\n"
#define DISABLE_GNSS_LOG_CMD  "DISABLE_LOG GNSS\n"
#define ENABLE_5MODE_LOG_CMD  "ENABLE_LOG 5MODE\n"
#define DISABLE_5MODE_LOG_CMD  "DISABLE_LOG 5MODE\n"
#define ENABLE_AGDSP_LOG_CMD  "ENABLE_LOG AG-DSP\n"
#define DISABLE_AGDSP_LOG_CMD  "DISABLE_LOG AG-DSP\n"

#define LOG_TYPE_NAME_MODEM     '1'
#define LOG_TYPE_NAME_WCN       '2'
#define LOG_LOCATION_PC         '1'
#define LOG_LOCATION_TCARD      '2'
#define LOG_LOCATION_NONE       '0'

class CCPCtl {
    public:
        pthread_mutex_t g_time_sync_lock;
        int m_fdSockSrv;

        char m_socket_rbuf[ENG_DATA_LENGTH];
        char m_socket_wbuf[ENG_DATA_LENGTH];


        CCPCtl();
        ~CCPCtl();

        void attach(CChnlMgr* lpChnlMgr);
        void run();

        static int logLocation(char log_type, char location, int diagportswitch);
        static void regCallBack(CModuleMgr* pModMgr);
        static int CpCtlHandle(char* buff, char* rsp);

    private:
        static CChnlMgr* m_lpChnlMgr;

        static int get_response(int fd, size_t timeout);
        static int notice_slogmodem(char *cmd);

        //cp log ctrl
        static void* cplogctl(void *arg);

        static int create_socket_local_server(CCPCtl* lpCpLogCtl);
        static void eng_socket_rw_thread(void *fd, void *arg);

        //time sync
        static void* timesync(void* arg);

        static int get_timezone();
        static enum CPEVENT_E parse_cp_event_notify(const char* buf);
        static void fds_init(struct pollfd *fds, int len);
        static void fds_collate(struct pollfd *fds, int len,
                        struct SocketConnection* modemd_sock,
                        struct SocketConnection* refnotify_sock);
        static int try_connect(struct SocketConnection* sock, struct pollfd* poll_array,
                       int len, int* poll_num, const char* server_name);
        static int add_connection(struct pollfd* poll_array, int len,
                          int* poll_num, int fd);
        static int cp_event_notify_rsp_handle(CP_EVENT_NOTIFY_T *cp_event);
        static int time_sync_rsp_handle(MODEM_TIMESTAMP_T *ts);
        static void current_ap_time_stamp_handle(TIME_SYNC_T *time_sync, MODEM_TIMESTAMP_T *time_stamp);
};


#endif