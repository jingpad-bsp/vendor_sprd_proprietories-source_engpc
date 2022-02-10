#ifndef __CPROTOL_H__
#define __CPROTOL_H__

#include "CTrans.h"

class CProtol:public CTrans{
    public:
        CProtol();
        virtual ~CProtol();

        virtual int decode(char* buff, int nlen);
        virtual int encode(char* buff, int nlen);

};

#endif