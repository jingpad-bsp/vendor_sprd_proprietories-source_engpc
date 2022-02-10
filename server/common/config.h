#ifndef __ENG_CONFIG_H__
#define __ENG_CONFIG_H__

#include "eng_vector.h"

class EngConf{
    public:
        enum CONF{
            CONF_DEV,
            CONF_CHNL,
    
            CONF_UNKNOWN
        };

        EngVector< EngVector<char*>* > m_cnt;

        EngConf();
        ~EngConf();

         int parse(int type, char* path);

    private:
        char* get_token(char* buf, int* len);
        EngVector<char*>* parse_line(char* buf, int line_num);
        int parse_config_file(const char* path);
        int get_file_size(const char *path);
};


#endif