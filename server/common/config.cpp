#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>

#include "eng_vector.h"

#include "config.h"
#include "englog.h"

EngConf::EngConf(){

}

EngConf::~EngConf(){
}

int EngConf::get_file_size(const char *path){
    int filesize = -1;
    struct stat statbuff;
    if(stat(path, &statbuff) < 0){
        return filesize;
    }else{
        filesize = statbuff.st_size;
    }

    return filesize;
}

char* EngConf::get_token(char* buf, int* len){
    while (*buf) {
        char c = *buf;
        if ('\t' != c && '\r' != c && '\n' != c) {
            break;
        }
        ++buf;
    }

    if (!*buf) {
        return NULL;
    }

    const char* p = buf + 1;
    while (*p) {
        char c = *p;
        if ('\t' == c || '\r' == c || '\n' == c || '\0' == c) {
            break;
        }
        ++p;
    }
    *len = p - buf;

    return buf;
}

EngVector<char*>* EngConf::parse_line(char* buf, int line_num){
    char* t1 = NULL;
    char* item = NULL;
    int tlen = 0;
    bool bValid = false;
    EngVector<char*> *vt = new EngVector<char*>();

    while(1){
        t1 = get_token(buf, &tlen);
        if (!t1) {
            break;
        }
        if ('#' == *t1) {
            delete vt;
            return NULL;
        }

        bValid = true;
        item = (char*)malloc(tlen+1);
        memset(item, 0, tlen+1);
        memcpy(item, t1, tlen);
        buf = t1 + tlen;

        vt->push_back(item);
    }

    if (!bValid) {
        delete vt;
        vt = NULL;
    }

    return vt;
}

int EngConf::parse_config_file(const char* path){
    FILE* pf = fopen(path,"rt");
    if (!pf) {
        return -1;
    }

    int line_num = -1;
    char buf[1024] = {0};

    while (true) {
        memset(buf, 0, sizeof(buf));
        char* p = fgets(buf, 1024, pf);
        if (!p) {
            break;
        }

        EngVector<char*>* pItem = parse_line(buf, line_num);
        if (NULL == pItem) {
            //EngLog::error("EngConf: invalid line: %s", buf);
            continue;
        }

        ++line_num;
        m_cnt.push_back(pItem);
    }
    fclose(pf);

    return 0;
}

int EngConf::parse(int type, char* path){
    EngLog::error("EngConf: parse %s ", path);
    if ((type != CONF_DEV && type != CONF_CHNL) || path == NULL){
        EngLog::error("EngConf: invalid type!");
        return -1;
    }

    if (access(path, R_OK) != 0){
        EngLog::error("EngConf: access %s fail.", path);
        return -1;
    }

    return parse_config_file(path);
}
