#ifndef __MISCDATA_H__
#define __MISCDATA_H__

int eng_read_miscdata(char *databuf, int data_len);
int eng_write_miscdata(char *databuf, int data_len);
int eng_read_miscdata_with_offset(int offset, char *databuf, int data_len);
int eng_write_miscdata_with_offset(int offset, char *databuf, int data_len);

#endif