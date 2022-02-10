#ifndef __CPROTOL_SMP_H__
#define __CPROTOL_SMP_H__

#include "CProtol.h"

#define AUDIO_DSP_LOG 0
#define AUDIO_DSP_PCM 1
#define AUDIO_DSP_MEM 2

class CProtolSmp:public CProtol{
    public:
        CProtolSmp();
        virtual ~CProtolSmp();

        virtual int decode(char* buff, int nlen);
        virtual int encode(char* buff, int nlen);

        void ag_dsplog_add_headers(uint8_t* buf, size_t len, unsigned type);

    private:
        int m_sn;
};

#endif