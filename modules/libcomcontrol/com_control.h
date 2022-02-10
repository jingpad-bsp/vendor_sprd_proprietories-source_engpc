#ifndef __COM_CONTROL_H__
#define __COM_CONTROL_H__

int open_diag_port (char *req, char *rsp);
int close_diag_port (char *req, char *rsp);
int cclk_hander(char* req,char* rsp);
int parse_number(const uint8_t* data,size_t len ,unsigned& num);
int parse_plus_cclk(char* str, int num);
int get_calend_time(time_t tnow, struct tm* lt, int* tz);
#endif
