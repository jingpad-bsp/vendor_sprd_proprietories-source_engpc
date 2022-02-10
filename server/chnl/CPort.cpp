#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <poll.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "CPort.h"
#include "frame.h"
#include "CTrans.h"

#define Info(fmt, args...) {\
        if (m_port.dataType != DATA_LOG) { \
            info(fmt, ##args); \
        } \
    }

//#define Info info

#define ZEROMEM(d,len) memset((void*)(d), 0, (len))
#define MEMCPY(d,s) { if ((void*)(s) != NULL) \
                          memcpy((void*)(d), (void*)(s), strlen(s)>=(sizeof(d)-1)?(sizeof(d)-1):(strlen(s))); \
                    }


CPort::CPort(char* devname, LPDEV_PORT lpDevPort){
    init(devname, lpDevPort->portName, lpDevPort->portType, lpDevPort->portPath, lpDevPort->dataType, lpDevPort->reserved, lpDevPort->desc);
}

CPort::CPort(char* devname, char* name, PORT_TYPE porttype, char* path, DATA_TYPE datatype, char* reserved, char* desc){
    init(devname, name, porttype, path, datatype, reserved, desc);
}

CPort::CPort(char* devname, char* name, char* porttype, char* path, char* datatype, char* reserved, char* desc){
    init(devname, name, str2PortType(porttype), path, str2DataType(datatype), reserved, desc);
}

CPort::~CPort(){
}

void CPort::init(char* devname, char* name, PORT_TYPE porttype, char* path, DATA_TYPE datatype, char* reserved, char* desc){
    if (name == NULL){
        return;
    }

    ZEROMEM(&m_port, sizeof(m_port));
    ZEROMEM(m_devName, sizeof(m_devName));

    MEMCPY(m_devName, devname);
    MEMCPY(m_port.portName, name);
    m_port.portType = porttype;
    MEMCPY(m_port.portPath, path);
    m_port.dataType = datatype;
    MEMCPY(m_port.reserved, reserved);

    MEMCPY(m_port.desc, desc);

    m_nClient = 0;
    m_bEnableRD = true;
    //memset(m_buff_RD, 0, MAX_PORT_BUFF_RD);
    m_bEnableWR = true;
    //memset(m_buff_WR, 0, MAX_PORT_BUFF_WR);
    m_fd = -1;
    m_mtx_rd = PTHREAD_MUTEX_INITIALIZER;
    m_mtx_wr = PTHREAD_MUTEX_INITIALIZER;
    m_mtx = PTHREAD_MUTEX_INITIALIZER;

    m_bSuspend = false;

    int fds[2] = {0};
    if(pipe(fds) == 0)
    {
        m_pipeMonitor = fds[0];
        m_pipeNotify = fds[1];
    }else{
        m_pipeMonitor = 0;
        m_pipeNotify = 0;
        info("create pipe fail");
    }
}

CTrans* CPort::attach(CTrans* lpTrans){
    CTrans* ptrLast = m_lpTrans;
    m_lpTrans = lpTrans;
    return ptrLast;
}

const char* CPort::TAG(){
/*
    if (m_lpTrans){
        sprintf(m_tag, "[%s][%s]", m_lpTrans->getname(), m_port.portName);
    }else{
        sprintf(m_tag, "[][%s]", m_port.portName);
    }
*/
    sprintf(m_tag, "[%s: %s]", m_devName, m_port.portName);

    return m_tag;
}

bool CPort::enable(bool bEnableRD, bool bEnableWR){
    info("enable: RD = %d, WR = %d", bEnableRD, bEnableWR);

    m_bEnableRD = bEnableRD;
    m_bEnableWR = bEnableWR;

    return true;
}

void CPort::getEnable(bool& bEnableRD, bool& bEnableWR){
    info("getEnable: RD = %d, WR = %d", m_bEnableRD, m_bEnableWR);

    bEnableRD = m_bEnableRD;
    bEnableWR = m_bEnableWR;
}

int CPort::read(char *buff, int nLen){
    int ret = 0;
    if (m_bEnableRD){
        pthread_mutex_lock(&m_mtx_rd);
        ret = internal_read(buff, nLen);
        pthread_mutex_unlock(&m_mtx_rd);
    }else{
        info("enable is false");
    }

    return ret;
}

int CPort::write(char *buff, int nLen){
    int ret = 0;
    if(m_bEnableWR){
        pthread_mutex_lock(&m_mtx_wr);
        ret = internal_write(buff, nLen);
        pthread_mutex_unlock(&m_mtx_wr);
    }else{
        info("enable is false");
    }

    return ret;
}

CPort* CPort::find(char* name){
    if (strncasecmp(name, m_port.portName, strlen(m_port.portName)) == 0) {
        return this;
    }else {
        return NULL;
    }
}

int CPort::open(){
    if(!m_bEnableRD){
        error("enable is false.");
        return -1;
    }

    if(m_port.portType == PORT_LOOP){
        info(" portType is PORT_LOOP");
        return 0;
    }

    if(m_port.portType == PORT_INTERFACE){
        info(" portType is PORT_INTERFACE");
        return 0;
    }

    if(m_port.portType > PORT_LOOP){
        error(" portType is PORT_UNKNOWN");
        return -1;
    }

    pthread_mutex_lock(&m_mtx);

    if (m_fd >= 0){
        info("%s alread opened, return m_fd = %d, m_nClient = %d", m_port.portPath, m_fd, m_nClient);
        m_nClient++;

        pthread_mutex_unlock(&m_mtx);
        return 0;
    }

    if(m_port.portType == PORT_SOCK){

    }else {
        int second = MAX_PORT_OPEN_TIMEOUT;
        info("open: %s", m_port.portPath);
        do {
            m_fd = ::open(m_port.portPath, O_RDWR);
            if (m_fd >= 0){
                break;
            }
            error("open fail! error = %s, sleep 1s, then retry!", strerror(errno));
            usleep(1000*1000);
        }while(second-->0);
        if (m_fd < 0){
            error("open %s fail,  error = %s", m_port.portPath, strerror(errno));

            pthread_mutex_unlock(&m_mtx);
            return -1;
        }
        info("open succ: m_fd = %d, portType = %s", m_fd, PortType2str(m_port.portType));
        if (m_port.portType == PORT_GSER || m_port.portType == PORT_VSER || m_port.portType == PORT_COM){
            struct termios ser_settings;
            if (isatty(m_fd)) {
                tcgetattr(m_fd, &ser_settings);
                cfmakeraw(&ser_settings);
                tcsetattr(m_fd, TCSANOW, &ser_settings);
            }
        }
        m_nClient=1;
    }

    pthread_mutex_unlock(&m_mtx);
    return 0;
}
int CPort::close(){
    Info("%s: m_nClient = %d, m_fd = %d", __FUNCTION__, m_nClient, m_fd);

    pthread_mutex_lock(&m_mtx);

    if (m_fd >= 0){
        m_nClient--;
        if (m_nClient <= 0){
            ::close(m_fd);
            m_fd = -1;
            m_nClient=0;
        }
    }

    pthread_mutex_unlock(&m_mtx);
    return 0;
}


int CPort::start(){
    if(m_bEnableRD){
    }
    if(m_bEnableWR){
    }

    return 0;
}

int CPort::stop(){
    if (m_bSuspend){
        notify(MSG_NOTIFY_EXIT, strlen(MSG_NOTIFY_EXIT));
    }

    return 0;
}

int CPort::reset(){
    stop();
    start();
    return 0;
}

int CPort::reopen(int second){
    Info("%s: m_nClient = %d", __FUNCTION__, m_nClient);
    int nCount = m_nClient;
    m_nClient = 1;
    close();
    while(second-- > 0 && 0 != open()){
        sleep(1);
    }

    if (second > 0){
        notify(MSG_NOTIFY_UPDATE_FD, strlen(MSG_NOTIFY_UPDATE_FD));
    }
    m_nClient = nCount;

    return second>0?0:-1;
}

void CPort::notify(char* msg, int len){
    Info("%s: msg = %s", __FUNCTION__, msg);
    if (m_pipeNotify >= 0){
        int ret = ::write(m_pipeNotify, msg, len);
        if (ret != len){
            Info("notify fail: ret = %d", ret);
        }
    }
}

int CPort::internal_read(char* buff, int nLen){
    int rdSize = -1;
    int r_cnt = 0;
    int offset = 0;
    int max_len = MAX_PORT_BUFF_RD;
    if (m_fd < 0){
        error("invalid  fd! please open first.");
        return -1;
    }

    if(m_port.portType == PORT_LOOP){
        error(" portType is PORT_LOOP");
        return 0;
    }

    if (m_port.portType == PORT_GSER || m_port.portType == PORT_VSER || m_port.portType == PORT_COM){
        max_len = MAX_VSER_BUFF_LEN;
    }

    rdSize = max_len>nLen?nLen:max_len;

    fd_set readfd = {0};

    memset(buff, 0, nLen);
    //to do
    do{
        int ret = 0;
        int max_fd =0;
        FD_ZERO(&readfd);
        FD_SET(getFD(),&readfd);
        if (m_pipeMonitor >= 0){
            FD_SET(m_pipeMonitor, &readfd);
        }

        m_bSuspend = true;
        Info("read select...m_fd = %d, m_pipeMonitor = %d", m_fd, m_pipeMonitor);
        max_fd = getFD()>m_pipeMonitor?getFD():m_pipeMonitor;
        ret = select(max_fd+1,&readfd,NULL,NULL,NULL);
        //Info("select return = %d", ret);
        if(ret == -1){
            error("select error");
            usleep(500*1000);
            continue;
        }else if(ret == 0){
            error("select time out");
        }else{
            if (FD_ISSET(m_pipeMonitor, &readfd)){
                char msg[MAX_MSG_LEN] = {0};
                Info("read notify...");
                r_cnt = ::read(m_pipeMonitor, msg, sizeof(msg));
                Info("msg = %s", msg);
                if (strcasecmp(msg, MSG_NOTIFY_EXIT) == 0){
                    offset = 0;
                    break;
                }else{
                    continue;
                }
            }else if(FD_ISSET(getFD(),&readfd)){// read
                int tmpOffset = 0;
                Info("read...");
                r_cnt = ::read(getFD(), buff+offset, rdSize);
                Info("===> recv size = %d", r_cnt);
                printData(buff, r_cnt, 20, 1);

                if (r_cnt <= 0){
                    error("read fail: %s", strerror(errno));
                    if (errno == EBUSY) {
                        usleep(59000);
                        continue;
                    }
                    if (0 == reopen(MAX_PORT_OPEN_TIMEOUT)){
                        Info("reopen succ");
                        continue;
                    }else{
                        error("reopen %s fail: %s", m_port.portName, strerror(errno));
                        break;
                    }
                }
                offset += r_cnt;

                // complete frame
                tmpOffset = offset;
                FRAME_TYPE type = m_lpTrans->checkframe(buff, offset);
                if (tmpOffset != offset){
                    Info("check frame: offset = %d", offset);
                    printData(buff, offset, 20, 1);
                }

                if (type == FRAME_COMPLETE || type == FRAME_INVALID){
                    break;
                }else if (type == FRAME_HALF_CONTINUE){
                    warn("this frame is half, continue read ...");
                }else{
                    warn("this frame type is unknown");
                }

                // not invalid frame && buff is full, break
                if (offset >= nLen){
                    warn("buff is full");
                    break;
                }

                // not invalid frame, so contine to read
                rdSize = nLen-offset;
                rdSize = rdSize>max_len?max_len:rdSize;

                continue;
            }
        }
    }while(1);

    m_bSuspend = false;
    return offset;
}

int CPort::internal_write(char* buff, int nLen){
    int offset = 0;
    int w_cnt = 0;
    bool bsplite = false;
    //Add timeout 60s
    struct timeval timeout;
    timeout.tv_sec = 60;
    timeout.tv_usec = 0;
    bool use_timeout = false;
    if (m_port.portType == PORT_GSER || m_port.portType == PORT_VSER || m_port.portType == PORT_COM){
        use_timeout = true;;
    }
    if (m_fd < 0){
        error("invalid  fd! please open first.");
        return -1;
    }

    if(m_port.portType == PORT_LOOP){
        error(" portType is PORT_LOOP");
        return nLen;
    }

    if ((nLen%64 == 0) && (m_port.portType == PORT_GSER || m_port.portType == PORT_VSER || m_port.portType == PORT_COM)){
        bsplite = true;;
    }

    fd_set writefd = {0};

    do {
        int ret = 0;
        int max_fd = 0;
        FD_ZERO(&writefd);
        FD_SET(getFD(),&writefd);

        Info("write select...m_fd = %d", m_fd);
        ret = select(getFD()+1,NULL,&writefd,NULL,use_timeout ? &timeout : NULL);
        Info("select return = %d", ret);
        if(ret == -1){
            error("select error");
            usleep(500*1000);
            continue;
        }else if(ret == 0){
            error("select time out");
            return 0;
        }else{
            if(FD_ISSET(getFD(),&writefd)){// write

                Info("<=== send size = %d", nLen);
                printData(buff, nLen, 20, 1);
                if (bsplite)
                    w_cnt = ::write(getFD(), buff+offset, nLen - 32);
                else
                    w_cnt = ::write(getFD(), buff+offset, nLen);
                Info("write w_cnt = %d", w_cnt);
                if (w_cnt < 0) {
                    error("write fail: %s", strerror(errno));
                    if (errno == EBUSY) {
                        usleep(59000);
                        continue;
                    }
                    if (0 == reopen(MAX_PORT_OPEN_TIMEOUT)){
                        warn("reopen succ");
                        continue;
                    }else{
                        error("reopen %s fail: %s", m_port.portName, strerror(errno));
                        break;
                    }
                }else{
                  nLen -= w_cnt;
                  offset += w_cnt;
                  bsplite = false;
                }
            }
        }
    }while(nLen > 0);

    return offset;
}

void CPort::print(){
    info("%s, %s, %s, %s, %s, %s", 
        m_port.portName, 
        PortType2str(m_port.portType), 
        m_port.portPath,
        DataType2str(m_port.dataType),
        m_port.reserved,
        m_port.desc);
}

void CPort::printData(char* buff, int len, int col, int brk){
    if (m_port.dataType == DATA_AT || m_port.dataType == DATA_WCN_AT){
        Info("print data: %s\n", buff);
    }else{
        int i = 0;
        int j = 0;
        char tmpbuf[128] = {0};

        len = len>(sizeof(tmpbuf)-1)?(sizeof(tmpbuf)-1):len;

        for(i = 0; i < len; i++) {
            sprintf(tmpbuf + j, "%02X ", *(buff + i));
            j = strlen(tmpbuf);
            if(i % col == col -1 || i == len -1) {
                Info("print data: %s\n",  tmpbuf);
                if (brk) {
                    break;
                } else {
                    j = 0;
                    continue;
                }
            }
        }
    }
}
