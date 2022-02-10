#ifndef  __ENG_LOG_H__
#define  __ENG_LOG_H__

#include "CModuleMgr.h"

typedef enum {
  LOG_NONE,
  LOG_ERR,
  LOG_WARN,
  LOG_DEBUG,
  LOG_INFO,

  LOG_UNKNOWN
}ENGPC_LOGLEVEL;


class EngLog{
    private:
        EngLog();
        ~EngLog();

        static ENGPC_LOGLEVEL s_loglevel;

    public:

        static void setLogLevel(ENGPC_LOGLEVEL level){ s_loglevel = level; }
        static ENGPC_LOGLEVEL getLogLevel() { return s_loglevel; }
        static void regCallBack(CModuleMgr* pModMgr);
        static int logLevelHandle(char* buff, char* rsp);

        static int error(const char* fmt, ...);
        static int info(const char* fmt, ...);
        static int warn(const char* fmt, ...);
        static int debug(const char* fmt, ...);
};


#endif