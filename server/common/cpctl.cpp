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

#include "cpctl.h"
#include "englog.h"
#include "adapter.h"
#include "sprd_fts_type.h"

extern int write_to_host_diag(char* buff, int len);
extern int chnl_send_at_interface(char* buff, int len);
#define AT_DIAG_OPEN    "AT+DIAGOPEN"
#define AT_DIAG_CLOSE   "AT+DIAGCLOSE"

int translate_packet(char *dest, char *src, int size) {
    int i;
    int translated_size = 0;
    
    dest[translated_size++] = 0x7E;
    
    for (i = 0; i < size; i++) {
        if (src[i] == 0x7E) {
            dest[translated_size++] = 0x7D;
            dest[translated_size++] = 0x5E;
        } else if (src[i] == 0x7D) {
            dest[translated_size++] = 0x7D;
            dest[translated_size++] = 0x5D;
        } else
            dest[translated_size++] = src[i];
    }
    dest[translated_size++] = 0x7E;
    return translated_size;
}

CChnlMgr* CCPCtl::m_lpChnlMgr = NULL;

CCPCtl::CCPCtl(){
    m_lpChnlMgr = NULL;
    m_fdSockSrv = -1;
    g_time_sync_lock = PTHREAD_MUTEX_INITIALIZER;

    memset(m_socket_rbuf, 0, sizeof(m_socket_rbuf));
    memset(m_socket_wbuf, 0, sizeof(m_socket_wbuf));
}

CCPCtl::~CCPCtl(){
}

void CCPCtl::attach(CChnlMgr* lpChnlMgr){
    m_lpChnlMgr = lpChnlMgr;
}

void CCPCtl::run(){
    pthread_t m_idTdLogCtl;
    pthread_t m_idTdTimeSync;
    pthread_attr_t attr;
    pthread_attr_init(&attr);

    if (0 != pthread_create(&m_idTdLogCtl, &attr, cplogctl, (void*)this)){
        EngLog::error("cp log ctrl fail.");
    }
    if (0 != pthread_create(&m_idTdTimeSync, &attr, timesync, (void*)this)){
        EngLog::error("cp time sync fail.");
    }
}

#define AT_SPATCPLOG "AT+SPATCPLOG"
#define AT_SPATCPLOG_TAG "+SPATCPLOG:"
void CCPCtl::regCallBack(CModuleMgr* pModMgr){
    struct eng_callback cb = {0};
    cb.diag_ap_cmd = -1;
    cb.type = 0xFF;
    cb.subtype = 0xFF;
    sprintf(cb.at_cmd, "%s", AT_SPATCPLOG);
    cb.eng_linuxcmd_func = CpCtlHandle;

    pModMgr->internalRegCallBack(&cb, 1);
}

int CCPCtl::CpCtlHandle(char* req, char* rsp){
    char *ptr = NULL;
    char cmd_buf[256] = {0};
    int ret = -1;
    int nlen = 0;
    if (NULL == req)
    {
        if(NULL == rsp){
            return 0;
        }else{
            return sprintf(rsp, "\r\nERROR\r\n");
        }
    }

    if(req[0] == 0x7e)
    {
        ptr = req + 1 + sizeof(MSG_HEAD_T);
    }
    else
    {
        ptr = strdup(req);
    }

    if (strncasecmp(ptr, AT_SPATCPLOG,strlen(AT_SPATCPLOG)) == 0){
        char *type;
        char log_type_cmd[2] = {0, 0};
        char log_dest_cmd[2] = {0, 0};
        int ret = 0;
        char modem_log_dest[8] = {0};
        char modem_log_type = LOG_TYPE_NAME_MODEM;
        char wcn_log_dest[8] = {0};
        char wcn_log_type = LOG_TYPE_NAME_WCN;
        int iDiagPortSwitch = 0;

        sys_getlogdest(&modem_log_type, modem_log_dest);
        sys_getlogdest(&wcn_log_type, wcn_log_dest);

        if (strchr(ptr, '?') != NULL) {
            if (strstr(ptr, "=1?")!=NULL) { // modem log_dest req
                sprintf(rsp, "%s MODEMLOG=%d\r\n", AT_SPATCPLOG_TAG, atoi(modem_log_dest));
            } else if (strstr(ptr, "=2?")!=NULL) {  // wcn log_dest req
                sprintf(rsp, "%s WCNLOG=%d\r\n", AT_SPATCPLOG_TAG, atoi(wcn_log_dest));
            } else {
                EngLog::error("%s: ERROR: invalid cmmond\n", __FUNCTION__);
                sprintf(rsp, "%s ERROR PARAM\r\n", AT_SPATCPLOG_TAG);
                goto out;
            }
        }else{
            char *temp = NULL;
            temp = strchr(ptr, '=');
            if(NULL == temp || NULL == temp + 1) {
                sprintf(rsp, "%s ERROR PARAM\r\n", AT_SPATCPLOG_TAG);
                EngLog::error("%s: ERROR: invalid cmmond\n", __FUNCTION__);
                goto out;
            }
            temp++;
            log_type_cmd[0] = *temp;
            temp = strchr(temp, ',');
            if(NULL == temp || NULL == temp + 1) {
                sprintf(rsp, "%s ERROR PARAM\r\n", AT_SPATCPLOG_TAG);
                EngLog::error("%s: ERROR: invalid cmmond\n", __FUNCTION__);
                goto out;
            }
            temp++;
            log_dest_cmd[0] = *temp;
            EngLog::info("%s: %d %d\n", __FUNCTION__,log_type_cmd[0], log_dest_cmd[0]);

            temp = strchr(temp, ',');
            if(NULL != temp && NULL != temp+1) {
                temp++;
                iDiagPortSwitch = *temp-'0';
                EngLog::info("%s: iDiagSwitch = %d\n", __FUNCTION__, iDiagPortSwitch);
            }

            ret = logLocation(log_type_cmd[0], log_dest_cmd[0], iDiagPortSwitch);
            if(ret == -1) {
                sprintf(rsp, "%s error %d\r\n", AT_SPATCPLOG_TAG, ret);
                EngLog::error("%s: logLocation return %d\n", __FUNCTION__, ret);
                goto out;
            }

            sprintf(rsp, "OK\r\n");
        }
    }else{
        sprintf(rsp, "%s unknown at\r\n", AT_SPATCPLOG_TAG);
    }

out:
    if(req[0] != 0x7e){
        free(ptr);
    }

    return strlen(rsp);
}

int CCPCtl::logLocation(char log_type, char location, int diagportswitch){
    int ret = 0;
    char bufLoc[8] = {0};
    char usb_conf[128] = {0};

    sys_getlogdest(&log_type, bufLoc);

    switch (log_type) {
    case LOG_TYPE_NAME_MODEM: // modem log
        switch (location) {
        case LOG_LOCATION_PC: // pc
            // start cp log
            EngLog::info("%s: enable cp log\n", __FUNCTION__);
            if (LOG_LOCATION_PC != *bufLoc) { //2 to 1 , 0 to 1
                if (notice_slogmodem(DISABLE_5MODE_LOG_CMD) < 0) {
                    ret = -1;
                } else { // notice slogomodem success
                    m_lpChnlMgr->resetWithDevName(DEV_MODEM_NAME, true);
                    memset(bufLoc, 0, sizeof(bufLoc));
                    *bufLoc = LOG_LOCATION_PC;
                    sys_setlogdest(&log_type, bufLoc);
                    if (notice_slogmodem(DISABLE_AGDSP_LOG_CMD) < 0) {
                        EngLog::error("notify slogmodem to disable agdsp log fail!");
                    }
                }
            } else {
                EngLog::warn("modem log already to pc!");
                ret = -1;
            }
            break;

        case LOG_LOCATION_TCARD: // t card
            EngLog::info("%s: disable cp log\n", __FUNCTION__);
            if (LOG_LOCATION_TCARD != *bufLoc) { //1 to 2 , 0 to 2
                if (notice_slogmodem(ENABLE_5MODE_LOG_CMD) < 0) {
                    ret = -1;
                } else {
                    m_lpChnlMgr->resetWithDevName(DEV_MODEM_NAME, false);
                    memset(bufLoc, 0, sizeof(bufLoc));
                    *bufLoc = LOG_LOCATION_TCARD;
                    sys_setlogdest(&log_type, bufLoc);
                    if (notice_slogmodem(ENABLE_AGDSP_LOG_CMD) < 0) {
                        EngLog::error("notify slogmodem to enable agdsp log fail!");
                    }
                }
            } else {
                EngLog::warn("modem log already to t card!");
                ret = -1;
            }
        break;

        case LOG_LOCATION_NONE: // no log
            EngLog::info("%s: no log case\n", __FUNCTION__);
            if (LOG_LOCATION_NONE != *bufLoc) { //1 to 0 , 2 to 0
                if (notice_slogmodem(DISABLE_5MODE_LOG_CMD) < 0) {
                    ret = -1;
                } else { // notice slogomodem success
                    memset(bufLoc, 0, sizeof(bufLoc));
                    *bufLoc = LOG_LOCATION_NONE;
                    sys_setlogdest(&log_type, bufLoc);
                    if (notice_slogmodem(DISABLE_AGDSP_LOG_CMD) < 0) {
                        EngLog::error("notify slogmodem to disable agdsp log fail!");
                    }
                }
            } else {
                EngLog::warn("modem log already to no log!");
                ret = -1;
            }
        break;

        default:
            ret = -1;
        break;
    }
    break;

    case LOG_TYPE_NAME_WCN: // wcn log
        switch (location) {
        case LOG_LOCATION_PC: // pc
            // start engpcclientwcn
            if (LOG_LOCATION_PC != *bufLoc) { //2 to 1 , 0 to 1
                if (notice_slogmodem(DISABLE_WCN_LOG_CMD) < 0) {
                    ret = -1;
                } else {
                    m_lpChnlMgr->resetWithDevName(DEV_WCN_NAME, true);
                    m_lpChnlMgr->getModMgr()->bqb_vendor_open();
                    memset(bufLoc, 0, sizeof(bufLoc));
                    *bufLoc = LOG_LOCATION_PC;
                    sys_setlogdest(&log_type, bufLoc);
                    //system("start engpcclientwcn");
                }
            } else {
                EngLog::warn("wcn log already to pc!");
                ret = -1;
            }
        break;

        case LOG_LOCATION_TCARD: // t card
            // stop engpcclientwcn
            if (LOG_LOCATION_TCARD != *bufLoc) { //1 to 2 , 0 to 2
                if (notice_slogmodem(ENABLE_WCN_LOG_CMD) < 0) {
                    ret = -1;
                } else {
                    m_lpChnlMgr->resetWithDevName(DEV_WCN_NAME, false);
                    memset(bufLoc, 0, sizeof(bufLoc));
                    *bufLoc = LOG_LOCATION_TCARD;
                    sys_setlogdest(&log_type, bufLoc);
                    //system("stop engpcclientwcn");
                }
            } else {
                EngLog::warn("wcn log already to t card!");
                ret = -1;
            }
        break;

        case LOG_LOCATION_NONE: // t card
            // stop engpcclientwcn
            if (LOG_LOCATION_NONE != *bufLoc) { //1 to 0 , 2 to 0
                if (notice_slogmodem(DISABLE_WCN_LOG_CMD) < 0) {
                    ret = -1;
                } else {
                    memset(bufLoc, 0, sizeof(bufLoc));
                    *bufLoc = LOG_LOCATION_NONE;
                    sys_setlogdest(&log_type, bufLoc);
                }
            } else {
                EngLog::warn("wcn log already no log!");
                ret = -1;
            }
        break;

        default:
            ret = -1;
            break;
        }
    break;

    default:
        ret = -1;
        break;
    }

    if (diagportswitch == 1 && location == LOG_LOCATION_PC){
        eng_usb_config(usb_conf, sizeof(usb_conf));
        if (strcasecmp(usb_conf, ",gser") != 0){
            chnl_send_at_interface(AT_DIAG_OPEN, strlen(AT_DIAG_OPEN));
            sys_restart_self();
        }
    }else if (diagportswitch == 2 && location != LOG_LOCATION_PC){
        eng_usb_config(usb_conf, sizeof(usb_conf));
        if (strcasecmp(usb_conf, ",gser") == 0){
            chnl_send_at_interface(AT_DIAG_CLOSE, strlen(AT_DIAG_CLOSE));
            sys_restart_self();
        }
    }else{
    }

    EngLog::info("%s ret=%d", __FUNCTION__, ret);
    return ret;
}

void* CCPCtl::cplogctl(void *arg){
    CCPCtl* lpCpCtl = (CCPCtl*)arg;
    int sockfd = -1;

    /* creat socket server */
    if (-1 == create_socket_local_server(lpCpCtl)) {
        EngLog::error("eng_soc thread start error");
        return NULL;
    }

    while (1) {
        EngLog::info("%s eng_connect_fd=%d", __FUNCTION__, sockfd);
        if ((sockfd = ::accept(lpCpCtl->m_fdSockSrv, (struct sockaddr *)NULL, NULL)) == -1) {
            EngLog::error("accept socket error: %s(errno: %d)", strerror(errno), errno);
            continue;
        }

        EngLog::info("%s connected! eng_connect_fd:%d socket_name=%s", __FUNCTION__, sockfd, SOCK_CPLOGCTL_NAME);
        eng_socket_rw_thread(&sockfd, (void* )lpCpCtl);
    }

    out:
        ::close(lpCpCtl->m_fdSockSrv);
        lpCpCtl->m_fdSockSrv = -1;

    return NULL;
}

int CCPCtl::create_socket_local_server(CCPCtl* lpCpCtl) {
    lpCpCtl->m_fdSockSrv = socket_local_server(SOCK_CPLOGCTL_NAME, ANDROID_SOCKET_NAMESPACE_ABSTRACT, SOCK_STREAM);
    if (lpCpCtl->m_fdSockSrv < 0) {
        EngLog::error("open %s failed: %s\n", SOCK_CPLOGCTL_NAME, strerror(errno));
        return -1;
    }
    if (::listen(lpCpCtl->m_fdSockSrv, 4) < 0) {
        EngLog::error("listen %s failed: %s\n", SOCK_CPLOGCTL_NAME, strerror(errno));
        ::close(lpCpCtl->m_fdSockSrv);
        return -1;
    }
    return 0;
}

void CCPCtl::eng_socket_rw_thread(void *fd, void *arg)
{
    int soc_fd;
    int ret;
    int length;
    int type;
    fd_set readfds;
    char buf[2*ENG_DATA_LENGTH] = {0};
    ENG_DATA_T *eng_data_ptr = NULL;
    CCPCtl* lpCpCtl = (CCPCtl*)arg;

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
        memset(lpCpCtl->m_socket_rbuf,0,ENG_DATA_LENGTH);
        memset(lpCpCtl->m_socket_wbuf,0,ENG_DATA_LENGTH);
        if (FD_ISSET(soc_fd,&readfds)) {
            length = ::read(soc_fd,lpCpCtl->m_socket_rbuf,ENG_DATA_LENGTH);
            if (length <= 0) {
                EngLog::error("%s length = %d, break",__FUNCTION__, length);
                break;
            }
            EngLog::info("%s  socket_read_buf = %d %d %d",__FUNCTION__, lpCpCtl->m_socket_rbuf[0], lpCpCtl->m_socket_rbuf[1], lpCpCtl->m_socket_rbuf[2]);
            eng_data_ptr = (ENG_DATA_T *)(lpCpCtl->m_socket_rbuf);
            eng_data_ptr->result = logLocation(eng_data_ptr->log_type + '0', eng_data_ptr->location + '0', 1);
            memcpy(lpCpCtl->m_socket_wbuf,(char *)eng_data_ptr,ENG_DATA_LENGTH);
            EngLog::info("%s  socket_wbuf = %d %d %d",__FUNCTION__, lpCpCtl->m_socket_wbuf[0], lpCpCtl->m_socket_wbuf[1], lpCpCtl->m_socket_wbuf[2]);
            ::write(soc_fd,lpCpCtl->m_socket_wbuf,ENG_DATA_LENGTH);
            EngLog::info("%s -2-eng_connect_fd=%d", __FUNCTION__, soc_fd);
        }
        EngLog::info("%s -3-eng_connect_fd=%d", __FUNCTION__, soc_fd);
    }
    EngLog::info("%s CLOSE_SOCKET",__FUNCTION__);
    ::close(soc_fd);
}

TIME_SYNC_T g_time_sync = {0};

void* CCPCtl::timesync(void *arg){
    int ser_fd = -1;
    int mod_cnt = -1;
    int ref_cnt = -1;
    struct pollfd fds[FDS_SIZE];
    int modem_reset_flag = 0;
    int err_counter = 0;
    struct timespec ts = {1, 0};
    struct SocketConnection modemd_conn = { -1, -1, 0, 0 };
    struct SocketConnection refnotify_conn = { -1, -1, 0, 0 };
    int total_conn = 0;
    int fds_collate_flag = 0;
    CCPCtl* lpCpCtl = (CCPCtl*)arg;
    fds_init(fds, FDS_SIZE);

    EngLog::info("eng_timesync thread start\n");
    while (1) {
        MODEM_TIMESTAMP_T time_stamp;
        TIME_SYNC_T time_sync;
        struct timeval time_now;
        struct sysinfo sinfo;
        char notify_buf[256] = {0};
        CP_EVENT_NOTIFY_T cp_event;
        int ret = -1;
        int i = 0;
        int poll_ret = -1;
        int timeout = -1;

        if (fds_collate_flag) {
          fds_collate(fds, FDS_SIZE, &modemd_conn, &refnotify_conn);
          fds_collate_flag = 0;
        }

        if (modemd_conn.fd < 0) {
          try_connect(&modemd_conn, fds, FDS_SIZE, &total_conn, MODEM_SOCKET_NAME);
        }
        if (refnotify_conn.fd < 0) {
          try_connect(&refnotify_conn, fds, FDS_SIZE, &total_conn, TIME_SERVER_SOCK_NAME);
        }

        EngLog::info("%s: total_conn=%d\n", __FUNCTION__, total_conn);
        // If not all connections are established, set a timeout.
        if (!total_conn) { // All connections failed
            // Sleep for a while and retry
            nanosleep(&ts, NULL);
            continue;
        } else if (total_conn < FDS_SIZE) { // Some connection failed
            // Set a timeout
            timeout = 1000;
        } else { // All connections are established
            // Wait for all connections
            timeout = -1;
        }
        poll_ret = poll(fds, total_conn, timeout);
        if (poll_ret > 0) {
            for (i = 0; i < total_conn; ++i) {
                if (fds[i].revents & POLLIN) {
                    if (i == modemd_conn.index) {
                        mod_cnt = read(fds[i].fd, notify_buf, sizeof(notify_buf) - 1);
                        if (mod_cnt < 0) {
                            EngLog::error("%s: read error[%s], can't read time info\n", __FUNCTION__,
                            strerror(errno));
                        } else if (mod_cnt == 0) {
                            modemd_conn.fd = -1;
                            fds[i].fd = -1;
                            --total_conn;
                            fds_collate_flag = 1;
                            EngLog::error("%s: modemd socket disconnect\n", __FUNCTION__);
                        } else {
                            notify_buf[mod_cnt] = '\0';
                            EngLog::info("%s: notify_buf=%s\n", __FUNCTION__, notify_buf);
                            if(parse_cp_event_notify(notify_buf) == CE_RESET) {
                                modem_reset_flag = 1;
                                cp_event.subsys = SS_MODEM;
                                cp_event.event = CE_RESET;
                                ret = cp_event_notify_rsp_handle(&cp_event);
                                if(ret == -1) {
                                    EngLog::error("%s: Modem Reset, notice pc tool Error[%s]\n", __FUNCTION__,
                                    strerror(errno));
                                } else {
                                    EngLog::info("%s: Modem Reset, notice pc tool success\n", __FUNCTION__);
                                }
                            }
                        }

                    } else if (i == refnotify_conn.index) {
                        ref_cnt = read(fds[i].fd, &time_sync, sizeof(TIME_SYNC_T));
                        if (ref_cnt < 0) {
                            EngLog::error("%s: read error[%s], can't read time info\n", __FUNCTION__,
                            strerror(errno));
                            continue;
                        } else if (ref_cnt == 0) {
                            refnotify_conn.fd = -1;
                            fds[i].fd = -1;
                            --total_conn;
                            fds_collate_flag = 1;
                            EngLog::warn("%s: refnotify socket disconnect\n", __FUNCTION__);
                            continue;
                        } else if (ref_cnt != sizeof(TIME_SYNC_T)) {
                            EngLog::error("%s: read %d bytes, can't read time info\n", __FUNCTION__, ref_cnt);
                            continue;
                        } else {
                            EngLog::info("%s: receive time sync data from refnotify\n", __FUNCTION__);
                            pthread_mutex_lock(&(lpCpCtl->g_time_sync_lock));
                            memcpy(&g_time_sync, &time_sync, sizeof(TIME_SYNC_T));
                            pthread_mutex_unlock(&(lpCpCtl->g_time_sync_lock));

                            current_ap_time_stamp_handle(&time_sync, &time_stamp);
                            if (modem_reset_flag) {
                                modem_reset_flag = 0;
                                ret = time_sync_rsp_handle(&time_stamp);
                                if(ret == -1) {
                                    EngLog::error("%s: Time sync, notice pc tool Error[%s]\n", __FUNCTION__,
                                    strerror(errno));
                                } else {
                                    EngLog::info("%s: Time sync, notice pc tool success\n", __FUNCTION__);
                                }
                            }
                        }
                    }
                }
            }
        }
    }

out:
    EngLog::info("eng_timesync thread end\n");
    if (modemd_conn.fd >= 0) close(modemd_conn.fd);
    if (refnotify_conn.fd >= 0) close(refnotify_conn.fd);
    pthread_mutex_destroy(&(lpCpCtl->g_time_sync_lock));
    return 0;
}

enum CPEVENT_E CCPCtl::parse_cp_event_notify(const char* buf){
  if(strstr(buf, "P-ARM Modem Assert") != NULL) {
    return CE_ASSERT;
  }
  if(strstr(buf, "TD Modem Assert") != NULL) {
    return CE_ASSERT;
  }
  if(strstr(buf, "Modem Assert") != NULL) {
    return CE_ASSERT;
  }
  if(strstr(buf, "Modem Reset") != NULL) {
    return CE_RESET;
  }
  if(strstr(buf, "Modem Alive") != NULL) {
    return CE_ALIVE;
  }
  if(strstr(buf, "Modem Blocked") != NULL) {
    return CE_BLOCKED;
  }
  return CE_NONE;
}

void CCPCtl::fds_init(struct pollfd *fds, int len){
    int i;
    for (i = 0; i < len; i++) {
        fds[i].fd = -1;
        fds[i].events = POLLIN;
        fds[i].revents = 0;
    }
    return;
}

void CCPCtl::fds_collate(struct pollfd *fds, int len,
         struct SocketConnection* modemd_sock,
         struct SocketConnection* refnotify_sock){
    int i;
    int j;

    // Search for the first hole in the array
    for (i = 0; i < len; ++i) {
        if (fds[i].fd < 0) {
            break;
        }
    }

    if (i >= len -1) {
        // No hole
        return;
    }

    // Move all non-hole to i and onwards
    for (j = i + 1; j < len; j++) {
        int fd = fds[j].fd;
        if (fd >= 0) {
            fds[i].fd = fd;
            if (fd == modemd_sock->fd) {
                modemd_sock->index = i;
            } else if (fd == refnotify_sock->fd) {
                refnotify_sock->index = i;
            }
        }
        ++i;
    }

    return;
}

int CCPCtl::try_connect(struct SocketConnection* sock, struct pollfd* poll_array,
        int len, int* poll_num, const char* server_name){
  int ret = -1;

  //max_retry:max number of connection trials, 0 for no limit
  if(sock->max_retry && sock->try_num > sock->max_retry)
    return ret;

  sock->fd = socket_local_client(server_name,
      ANDROID_SOCKET_NAMESPACE_ABSTRACT,
      SOCK_STREAM);
  if (sock->fd >= 0) {
    // Add the connection to the polling array
    sock->index = add_connection(poll_array, len, poll_num, sock->fd);
    EngLog::info("%s: connect %s socket poll_num=%d, sock->fd=%d, sock->index=%d\n",
        __FUNCTION__, server_name, *poll_num, sock->fd, sock->index);
    // Reset the connection count
    sock->try_num = 0;
    ret = sock->index;
  } else { // Connection failed
    ++sock->try_num;
  }

  if (ret < 0) {
    EngLog::error("%s: connect %s socket error(%s), try_num=%d\n", __FUNCTION__,
        server_name, strerror(errno), sock->try_num);
  } else {
    EngLog::info("%s: connect %s socket success, try_num=%d\n", __FUNCTION__,
        server_name, sock->try_num);
  }
  return ret;
}

int CCPCtl::add_connection(struct pollfd* poll_array, int len, int* poll_num, int fd){
    // Search for the first available position
    int i;

    for (i = 0; i < len; ++i) {
        if (poll_array[i].fd < 0) {
            poll_array[i].fd = fd;
            poll_array[i].events = POLLIN;
            poll_array[i].revents = 0;
            ++*poll_num;
            break;
        }
    }
    if (i >= len) {
        i = -1;
    }
    // Return the index in the array
    return i;
}

int CCPCtl::cp_event_notify_rsp_handle(CP_EVENT_NOTIFY_T *cp_event){
    int ret = -1;
    char tmp[128] = {0};
    int tmp_len;
    char rsp[256] = {0};
    int rsp_len;
    MSG_HEAD_T head;
    head.len = sizeof(MSG_HEAD_T) + sizeof(CP_EVENT_NOTIFY_T);
    head.seq_num = 0;
    head.type = 0x33;
    head.subtype = 0xff;
    memcpy(tmp, &head, sizeof(MSG_HEAD_T));
    memcpy(tmp + sizeof(MSG_HEAD_T), cp_event, sizeof(CP_EVENT_NOTIFY_T));

    rsp_len = ::translate_packet(rsp, tmp, head.len);

    ret = write_to_host_diag(rsp, rsp_len);
    if (ret <= 0) {
        EngLog::error("%s: eng_diag_write2pc ret=%d !\n", __FUNCTION__, ret);
        return -1;
    }
    return 0;
}

int CCPCtl::time_sync_rsp_handle(MODEM_TIMESTAMP_T *ts){
  int ret = -1;
  char tmp[128] = {0};
  int tmp_len;
  char rsp[256] = {0};
  int rsp_len;
  MSG_HEAD_T head;
  TOOLS_DIAG_AP_CNF_T ap_cnf;
  head.len = sizeof(MSG_HEAD_T) + sizeof(TOOLS_DIAG_AP_CNF_T) + sizeof(MODEM_TIMESTAMP_T);
  head.seq_num = 0;
  head.type = 0x5;
  head.subtype = 0x11;
  memcpy(tmp, &head, sizeof(MSG_HEAD_T));
  ap_cnf.status = 0;
  ap_cnf.length = sizeof(MODEM_TIMESTAMP_T);
  memcpy(tmp + sizeof(MSG_HEAD_T), &ap_cnf, sizeof(TOOLS_DIAG_AP_CNF_T));
  memcpy(tmp + sizeof(MSG_HEAD_T) + sizeof(TOOLS_DIAG_AP_CNF_T), ts, sizeof(MODEM_TIMESTAMP_T));

  rsp_len = ::translate_packet(rsp, tmp, head.len);

  ret = write_to_host_diag(rsp, rsp_len);
  if (ret <= 0) {
    EngLog::error("%s: eng_diag_write2pc ret=%d !\n", __FUNCTION__, ret);
    return -1;
  }
  return 0;

}

void CCPCtl::current_ap_time_stamp_handle(TIME_SYNC_T *time_sync, MODEM_TIMESTAMP_T *time_stamp){
    struct timeval time_now;
    struct sysinfo sinfo;
    struct tm *tmp;
    char strTime[32] = {0};

    gettimeofday(&time_now, 0);
    sysinfo(&sinfo);
    time_stamp->sys_cnt = time_sync->sys_cnt + (sinfo.uptime - time_sync->uptime) * 1000;
    time_stamp->tv_sec = (uint32_t)(time_now.tv_sec);
    time_stamp->tv_usec = (uint32_t)(time_now.tv_usec);

    //add timezone
    time_stamp->tv_sec += get_timezone();
    EngLog::info("%s: [time_sync]:sys_cnt=%d, uptime=%d; "
            "[time_stamp]:tv_sec=%d, tv_usec=%d, sys_cnt=%d\n",
            __FUNCTION__, time_sync->sys_cnt, time_sync->uptime,
            time_stamp->tv_sec, time_stamp->tv_usec, time_stamp->sys_cnt);

    tmp = gmtime((const time_t*)(&time_stamp->tv_sec));
    strftime(strTime, 32, "%F %T", tmp);
    EngLog::info("%s: AP Time %s %d Micorseconds\n", __FUNCTION__, strTime, time_stamp->tv_usec);

    return;
}

int CCPCtl::get_timezone() {
    time_t t1;
    struct tm* pg;
    struct tm gm_tm;

    t1 = time(0);
    if ((time_t)(-1) == t1) {
        return 0;
    }
    pg = gmtime_r(&t1, &gm_tm);
    if (!pg) {
        return 0;
    }

    return (int)(difftime(t1, mktime(pg)));
}

int CCPCtl::get_response(int fd, size_t timeout) {
    int ret = -1;
    char resp[MAXLENRESP] = {0};
    struct pollfd r_pollfd;
    
    r_pollfd.fd = fd;
    r_pollfd.events = POLLIN;
    
    ret = poll(&r_pollfd, 1, timeout);
    if(ret < 0) {
        EngLog::error("poll slogmodem fail\n");
    } else {
        if (r_pollfd.revents & POLLIN) {
            if ((read(fd, resp, MAXLENRESP)) < 3 || memcmp(resp, "OK\n", 3)) {
                EngLog::error("err response from slogmodem\n");
            } else {
                ret = 1;
            }
        }
    }
    return ret;
}

int CCPCtl::notice_slogmodem(char *cmd)
{
    int ret = 0;
    int fd = -1;
    EngLog::info("notice_slogmodem: client connect...");
    fd = socket_local_client(SLOG_MODEM_SERVER_SOCK_NAME,
                             ANDROID_SOCKET_NAMESPACE_ABSTRACT, SOCK_STREAM);
    EngLog::info("notice_slogmodem: client connect succ!! fd = %d", fd);
    if (fd < 0) {
        EngLog::error("can't connect to slogmodem server, ERROR:%s\n", strerror(errno));
        return -1;
    }
    
    int flags = fcntl(fd, F_GETFL);
    flags |= O_NONBLOCK;
    int err = fcntl(fd, F_SETFL, flags);
    if (-1 == err) {
        EngLog::error("set slogmodem socket to O_NONBLOCK error\n");
        ret = -1;
        close(fd);
        return ret;
    }

    int len = write(fd, cmd, strlen(cmd));
    if (strlen(cmd) != len) {
        EngLog::error("FLUSH command write error, len=%d, ERROR:%s\n", len, strerror(errno));
        ret = -1;
        close(fd);
        return ret;
    }
    
    // Wait for the response for a while before failure.
    int result = get_response(fd, 3000);
    if (1 != result) {
        EngLog::error("ERROR: get response %d\n", result);
        ret = -1;
        close(fd);
        return ret;
    }

    close(fd);
    return ret;
}