#ifndef __CPROTOL_AT_H__
#define __CPROTOL_AT_H__

#include "CProtol.h"

class CProtolAT:public CProtol{
    public:
        CProtolAT();
        virtual ~CProtolAT();

        virtual int decode(char* buff, int nlen);
        virtual int encode(char* buff, int nlen);

        virtual int process(char* req, int reqlen, char* rsp, int rsplen, int& retlen);

};

#endif