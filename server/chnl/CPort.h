#ifndef __DEV_PORT_H__
#define __DEV_PORT_H__

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>

#include "dev.h"
#include "CBase.h"

#define MAX_VSER_BUFF_LEN (8*1024)
//For save r/w time BUFF%64==0 maybe write twice.
#define MAX_PORT_BUFF_RD (MAX_VSER_BUFF_LEN*8 - 16)
#define MAX_PORT_BUFF_WR (MAX_VSER_BUFF_LEN*8 - 16)

#define MAX_PORT_OPEN_TIMEOUT (5*60)

#define MAX_MSG_LEN 256

class CTrans;

class CPort:public CBase{
/*
    class Work{
        public:
            Work(int mode, int fd, int lock, char* pBuff, int size);
            ~Work();

            int start();
            int stop();

        private:
            int m_mode;
            int m_fd;
            int m_Lock;
            char* m_pBuff;
            int m_sizeBuff;
            pthread_t m_td;
            
    }
*/
    public:
        CPort(char* devname, LPDEV_PORT lpDevPort);
        CPort(char* devname, char* name, PORT_TYPE porttype, char* path, DATA_TYPE datatype, char* reserved, char* desc);
        CPort(char* devname, char* name, char* porttype, char* path, char* datatype, char* reserved, char* desc);
        virtual ~CPort();

        const char* TAG();
        void init(char* devname, char* name, PORT_TYPE porttype, char* path, DATA_TYPE datatype, char* reserved, char* desc);

        virtual int open();
        virtual int close();
        virtual int read(char *buff, int nLen);
        virtual int write(char *buff, int nLen);

        virtual int reopen(int second);

        int start();
        int stop();
        int reset();

        bool enable(bool bEnableRD, bool bEnableWR);
        void getEnable(bool& bEnableRD, bool& bEnableWR);

        CPort* find(char* name);
        const char* getname() { return m_port.portName; }
        const char* getpath() { return m_port.portPath; }
        const char* getDevName() { return m_devName; }
        int getFD() {
            pthread_mutex_lock(&m_mtx);
            int fd = m_fd;
            pthread_mutex_unlock(&m_mtx);
            return fd;
        }
        int getPortType() {return m_port.portType;}
        char* getPortTypeStr() {return PortType2str(m_port.portType);}
        CTrans* attach(CTrans* lpTrans);

        void print();
        void printData(char* buff, int len, int col, int brk);

    private:
        DEV_PORT m_port;
        char m_devName[DEV_NAME_LEN];
        char m_tag[CHNL_NAME_LEN+PORT_NAME_LEN];
        int  m_nClient;
        int  m_fd;
        bool m_bSuspend;
        CTrans *m_lpTrans;

        pthread_mutex_t m_mtx;

        bool m_bEnableRD;
        //char m_buff_RD[MAX_PORT_BUFF_RD];
        pthread_mutex_t m_mtx_rd;

        bool m_bEnableWR;
        //char m_buff_WR[MAX_PORT_BUFF_WR];
        pthread_mutex_t m_mtx_wr;

        //Work m_work_rd;
        //Work m_work_wr;

        int internal_read(char* buff, int nLen);
        int internal_write(char* buff, int nLen);
        PORT_TYPE StrToPortType(char* str);
        DATA_TYPE StrToDataType(char* str);

#define MSG_NOTIFY_UPDATE_FD "MSG_UPDATE_FD"
#define MSG_NOTIFY_EXIT "MSG_EXIT"
        void notify(char* msg, int len);
        int m_pipeMonitor;
        int m_pipeNotify;
};

#endif