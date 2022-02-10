#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include <getopt.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/types.h>

#include "sprd_fts_type.h"
#include "sprd_fts_log.h"

#define DIAG_HEADER_FLAG 0x7E
#define SPACE 0x20
#define QUESTION_MARK 0x3F
#define CARRIAGE_RETURN 0x0D

// return : rsp true length
int open_diag_port (char *req, char *rsp){

   ENG_LOG("dylib :enter function test :AT+DIAGOPEN!");
   property_set("persist.vendor.sys.modem.diag", ",gser");
   sprintf(rsp, "%s", "AT+DIAGOPEN open");
   return 0;
}
// return : rsp true length
int close_diag_port (char *req, char *rsp){
   ENG_LOG("dylib test :enter function test :AT+DIAGCLOSE!");
   property_set("persist.vendor.sys.modem.diag", "none");
sprintf(rsp, "%s", "AT+DIAGCLOSE close");
   return 0;
}

int parse_number(const uint8_t* data, size_t len, unsigned* num ) {
  const uint8_t* endp = data + len;
  *num = 0;
  while (data < endp) {
    int n = *data;
    if (n < '0' || n > '9') {
      return -1;
    }
    if (num >= UINT_MAX) {
      return -1;
    }
    n -= '0';
    *num = *num * 10 + n;
    ++data;
  }
  return 0;
}

int get_calend_time(time_t tnow, struct tm* lt, int* tz) {
  char tzs[16];
  int sign;
  unsigned n;
  int toffset;

  if (!localtime_r(&tnow, lt)) {
    return -1;
  }

  if (!strftime(tzs, sizeof tzs, "%z", lt)) {
    return 0;
  }

  ENG_LOG("%s tzs=%s", __FUNCTION__, tzs);
  // The time zone diff is in ISO 8601 format, i.e. one of:
  // +|-hhmm
  // +|-hh:mm
  //  +|-hh/
  if ('+' == tzs[0]) {
    sign = 1;
  } else if ('-' == tzs[0]) {
    sign = -1;
  } else {
    return 0;
  }

  if (parse_number((uint8_t*)(tzs + 1), 2, &n)) {
    return 0;
  }
  if (!tzs[3]) {
    toffset = 4 * (int)(n);
    *tz = toffset * sign;
    return 0;
  } else {  // Skip the ':' if one exists.
    toffset = 4 * (int)(n);
    const uint8_t* m=(uint8_t*)(tzs + 3);
    if (':' == tzs[3]) {
      ++m;
    }
    if (parse_number(m, 2, &n)) {
      return 0;
    }
    toffset +=(int)(n)/15;
    *tz = toffset * sign;
    return 1;
  }
}

int parse_plus_cclk(char* str, int num) {
  int at_cmd_len = strlen("AT+CCLK");
  str += at_cmd_len;
  num -= at_cmd_len;
  char* endp = str + num;

  ENG_LOG("%s: str=%s num=%d",  __FUNCTION__, str, num);
  while (str < endp) {
    if (SPACE != *str) {
      break;
    }
    str++;
  }

  if (str == endp) {
    return 0;
  }
  if (QUESTION_MARK == *str) {
    str++;
    while (str < endp) {
      if (SPACE != *str && CARRIAGE_RETURN != *str) {
        return 0;
      } else if(CARRIAGE_RETURN == *str) {
        break;
      }
      str++;
    }
  } else {
    return 0;
  }

  return 1;
}

int  cclk_hander(char* req, char* rsp){
  char str[]="+CCLK:";
  char err[]="+CME ERROR:";
  char* ptr;
  int num;
  MSG_HEAD_T msg_head;
  time_t t = time(0);
  struct tm lt;
  int tz, res;

  if (NULL == req) {
    return ENG_DIAG_RET_UNSUPPORT;
  }

  //formats of AT frame are different when connecting different ports
  //when connect lte diag port, AT format is diag AT:
  //" 7E AA 12 74 00 11 00 68 00 41 54 2B 43 43 4C 4B 3F 0D 7E"
  if (req[0] == DIAG_HEADER_FLAG) {
    memcpy(&msg_head, &req[1], sizeof(MSG_HEAD_T));
    num = msg_head.len - sizeof(MSG_HEAD_T);
    ptr = req + sizeof(MSG_HEAD_T) + 1 ;
  } else {//when connect lte AT port, AT format is naked data
    ptr = req;
    num = strlen(req);
  }

  if (parse_plus_cclk(ptr, num) == 1) {
    res = get_calend_time(t, &lt, &tz);
    ENG_LOG("res=%d, tz=%d", res, tz);
    if (res == 1) {
      sprintf(rsp, "%s%04d/%02d/%02d,%02d:%02d:%02d%+03d", str, lt.tm_year + 1900,
              lt.tm_mon + 1, lt.tm_mday, lt.tm_hour, lt.tm_min, lt.tm_sec, tz);
    } else if (res == 0) {
      sprintf(rsp, "%s%04d/%02d/%02d,%02d:%02d:%02d", str, lt.tm_year + 1900,
              lt.tm_mon + 1, lt.tm_mday, lt.tm_hour, lt.tm_min, lt.tm_sec);
    } else {
      sprintf(rsp,"%s%d", err, 4);
    }
    return strlen(rsp);
  } else {
    return ENG_DIAG_RET_UNSUPPORT;
  }
}

//最多添加32条AT指令
//num：实际注册的AT指令条数
void register_this_module_ext(struct eng_callback *reg, int *num)
{
    int moudles_num = 0;
    ENG_LOG("register_this_module_ext :dllibcomcontrol");

    //1st command
    sprintf(reg->at_cmd, "%s", "AT+DIAGOPEN");
    reg->eng_linuxcmd_func = open_diag_port;
    moudles_num++;

    //2nd command
    sprintf((reg+1)->at_cmd, "%s", "AT+DIAGCLOSE");
    (reg+1)->eng_linuxcmd_func = close_diag_port;
    moudles_num++;

    //3rd command
    sprintf((reg+2)->at_cmd, "%s", "AT+CCLK");
    (reg+2)->eng_linuxcmd_func = cclk_hander;
    moudles_num++;

    *num = moudles_num;
    ENG_LOG("register_this_module_ext: %d - %d",*num, moudles_num);
}
