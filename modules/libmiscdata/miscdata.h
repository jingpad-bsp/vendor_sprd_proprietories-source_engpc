#ifndef __MISCDATA_H__
#define __MISCDATA_H__

int eng_read_miscdata(char *databuf, int data_len);
int eng_write_miscdata(char *databuf, int data_len);
int eng_read_miscdata_with_offset(int offset, char *databuf, int data_len);
int eng_write_miscdata_with_offset(int offset, char *databuf, int data_len);

int eng_diag_product_ctrl(char *buf, int len, char *rsp, int rsplen);
int eng_diag_product_ctrl_ex(char *buf, int len, char *rsp, int rsplen);

#endif