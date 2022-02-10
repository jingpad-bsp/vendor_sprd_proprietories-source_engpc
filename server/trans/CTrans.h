#ifndef __ENG_CTRANS_H__
#define __ENG_CTRANS_H__

#include <stdio.h>

#include "channel.h"
#include "frame.h"

#define MAX_CHNL_BUFF  (64*1024)

class CPort;
class CChnlThread;

class  CTrans{
    public:
        int m_dataType;
        int m_retType;
        FRAME_TYPE m_frameType;

        CTrans();
        virtual ~CTrans();

        virtual int trans();

        virtual int pre_read(char* buff, int nlen);
        virtual int read(char* buff, int nlen);
        virtual int post_read(char* buff, int nlen, int nrecv);
        virtual int pre_write(char* buff, int nlen);
        virtual int write(char* buff, int nlen);
        virtual int post_write(char* buff, int nlen, int nsend);

        virtual int process(char* req, int reqlen, char* rsp, int rsplen, int& retlen);
        
        virtual int decode(char* buff, int nlen);
        virtual int encode(char* buff, int nlen);

        virtual FRAME_TYPE checkframe(char* buff, int& nlen);
        virtual int findframe(char* buff, int nlen);

        void init(char* name, CPort* lpSrc, CPort* lpDst, int dataType, int apProces);

        char* getname(){return m_name;}
        CPort* getPortSrc(){return m_lpPortSrc;}
        CPort* getPortDst(){return m_lpPortDst;}
        int getApProcess() {return m_apProcess;}
        void attach(CChnlThread* lptd){ m_lpChnlThread = lptd; }
        void updatePortDst(CPort* lpDst) { m_lpPortDst = lpDst; }
        int error(const char* fmt, ...);
        int info(const char* fmt, ...);
        int warn(const char* fmt, ...);
        int debug(const char* fmt, ...);
        virtual const char* TAG(){ sprintf(m_tag, "[%s]", m_name); return m_tag;}

    private:
        CPort* m_lpPortSrc;
        CPort* m_lpPortDst;
        char m_name[CHNL_NAME_LEN];
        char m_tag[CHNL_NAME_LEN+2];
        int m_apProcess;

        int m_nLenReq;
        char m_chnl_buff_req[MAX_CHNL_BUFF];
        int m_nLenRsp;
        char m_chnl_buff_rsp[MAX_CHNL_BUFF];
        char m_chnl_buff_req_bk[MAX_CHNL_BUFF];

        CChnlThread* m_lpChnlThread;

        void activeOtherChnlDstPort();
};


#endif