#ifndef __CPROTOL_DIAG_H__
#define __CPROTOL_DIAG_H__

#include "CProtol.h"
#include <stdbool.h>

#define PENDING_MARK "\r\nPENDING\r\n"

class CProtolDiag:public CProtol{
    public:
        CProtolDiag();
        virtual ~CProtolDiag();

        virtual int decode(char* buff, int nlen);
        virtual int encode(char* buff, int nlen);
        virtual FRAME_TYPE checkframe(char* buff, int& nlen);
        virtual int findframe(char* buff, int nlen);
        virtual int process(char* req, int reqlen, char* rsp, int rsplen, int& retlen);

        bool checkPending(char*rsp, int nlen);
        void setPendingMark(bool bPending){m_bPending = bPending;}

        static int translate_packet(char *dest, char *src, int size);
    public:
        int m_nDiagAT;
        bool m_bPending;
        FRAME_TYPE m_iCurFrameType;

    private:
        int findFrameFlag(char* buff, int nlen);
};

#endif
