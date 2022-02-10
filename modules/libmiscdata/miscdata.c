#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include <getopt.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <cutils/properties.h>

#ifdef CONFIG_NAND
#include <sys/ioctl.h>
#include <ubi-user.h>
#endif

#include "crc16.h"
#include "sprd_fts_diag.h"
#include "sprd_fts_log.h"
#include "phasecheck.h"
#include "translate_packet.h"

#define NAND_MISCDATA_FILESIZE (1024*1024)

#define PROP_MISCDATA_PATH "ro.vendor.product.partitionpath"

unsigned long get_file_size(const char *path);

int eng_read_miscdata(char *databuf, int data_len) {
  int ret = 0;
  int len;
  char prop[128] = {0};
  char miscdata_path[128] = {0};
  int fd = -1;

  if (-1 == property_get(PROP_MISCDATA_PATH, prop, "")){
    ENG_LOG("%s: get partitionpath fail\n", __FUNCTION__);
    return -1;
  }

  sprintf(miscdata_path, "%smiscdata", prop);
  fd = open(miscdata_path, O_RDONLY);
  if (fd >= 0) {
    ENG_LOG("%s open Ok miscdata_path = %s ", __FUNCTION__,
            miscdata_path);
    len = read(fd, databuf, data_len);

    if (len <= 0) {
      ret = -2;
      ENG_LOG("%s read fail miscdata_path = %s ", __FUNCTION__,
              miscdata_path);
    }
    close(fd);
  } else {
    ENG_LOG("%s open fail miscdata_path = %s ", __FUNCTION__,
            miscdata_path);
    ret = -3;
  }
  return ret;
}

int eng_write_miscdata(char *databuf, int data_len) {
  int ret = 0;
  int len;
  int fd = -1;
  char prop[128] = {0};
  char miscdata_path[128] = {0};

  if (-1 == property_get(PROP_MISCDATA_PATH, prop, "")){
    ENG_LOG("%s: get partitionpath fail\n", __FUNCTION__);
    return -1;
  }

  sprintf(miscdata_path, "%smiscdata", prop);
  fd = open(miscdata_path, O_WRONLY);
  if (fd >= 0) {
    ENG_LOG("%s open Ok miscdata_path = %s ", __FUNCTION__,
            miscdata_path);
#ifdef CONFIG_NAND
    __s64 up_sz = data_len;
    ioctl(fd, UBI_IOCVOLUP, &up_sz);
#endif
    len = write(fd, databuf, data_len);

    if (len <= 0) {
      ret = -2;
      ENG_LOG("%s read fail miscdata_path = %s ", __FUNCTION__,
              miscdata_path);
    }
    fsync(fd);
    close(fd);
  } else {
    ENG_LOG("%s open fail miscdata_path = %s ", __FUNCTION__,
            miscdata_path);
    ret = -3;
  }
  return ret;
}


int eng_read_miscdata_with_offset(int offset, char *databuf, int data_len) {
  int ret = 0;
  int len;
  int fd = -1;
  char prop[128] = {0};
  char miscdata_path[128] = {0};
  int szFile = 0;
  char* buff = NULL;

  if (-1 == property_get(PROP_MISCDATA_PATH, prop, "")){
    ENG_LOG("%s: get partitionpath fail\n", __FUNCTION__);
    return -1;
  }

  sprintf(miscdata_path, "%smiscdata", prop);

#ifdef CONFIG_NAND
  szFile = get_file_size(miscdata_path);
  ENG_LOG("%s: szFile = %d", __FUNCTION__, szFile);
  if(szFile <= 0){
    return -2;
  }

  buff = malloc(szFile+1);
  if (buff == NULL){
    ENG_LOG("%s: malloc fail\n", __FUNCTION__);
    return -3;
  }
  memset(buff, 0, szFile+1);
  ret = eng_read_miscdata(buff, szFile);
  if (0 == ret){
    memcpy(databuf, buff+offset, data_len);
  }
  free(buff);
  buff = NULL;

  return ret;
#else
  fd = open(miscdata_path, O_RDONLY);
  if (fd >= 0) {
    ENG_LOG("%s open Ok miscdata_path = %s ", __FUNCTION__,
            miscdata_path);
    if (lseek(fd, offset, SEEK_SET) == -1) {
      ENG_LOG("%s offset:%d seek failed! = %s ", __FUNCTION__, offset,
              miscdata_path);
      ret = -4;
      close(fd);
      return ret;
    }
    len = read(fd, databuf, data_len);

    if (len <= 0) {
      ret = -2;
      ENG_LOG("%s read fail miscdata_path = %s ", __FUNCTION__,
              miscdata_path);
    }
    close(fd);
  } else {
    ENG_LOG("%s open fail miscdata_path = %s ", __FUNCTION__,
            miscdata_path);
    ret = -3;
  }
#endif

  return ret;
}

int eng_write_miscdata_with_offset(int offset, char *databuf, int data_len) {
  int ret = 0;
  int len;
  int fd = -1;
  char prop[128] = {0};
  char miscdata_path[128] = {0};
  int szFile = 0;
  char* buff = NULL;

  if (-1 == property_get(PROP_MISCDATA_PATH, prop, "")){
    ENG_LOG("%s: get partitionpath fail\n", __FUNCTION__);
    return -1;
  }

  sprintf(miscdata_path, "%smiscdata", prop);

#ifdef CONFIG_NAND
  szFile = get_file_size(miscdata_path);
  ENG_LOG("%s: szFile = %d", __FUNCTION__, szFile);
  if(szFile <= 0){
    return -2;
  }

  buff = malloc(szFile+1);
  if (buff == NULL){
    ENG_LOG("%s: malloc fail\n", __FUNCTION__);
    return -3;
  }
  memset(buff, 0, szFile+1);
  ret = eng_read_miscdata(buff, szFile);
  if(0 == ret){
    memcpy(buff+offset, databuf, data_len);
    ret = eng_write_miscdata(buff, szFile);
  }else{
    ret = -4;
  }
  free(buff);
  return ret;

#else
  fd = open(miscdata_path, O_WRONLY);
  if (fd >= 0) {
    ENG_LOG("%s open Ok miscdata_path = %s ", __FUNCTION__,
            miscdata_path);
    if (lseek(fd, offset, SEEK_SET) == -1) {
      ENG_LOG("%s offset:%d seek failed! = %s ", __FUNCTION__, offset,
              miscdata_path);
      ret = -4;
      close(fd);
      return ret;
    }
    len = write(fd, databuf, data_len);

    if (len <= 0) {
      ret = -2;
      ENG_LOG("%s read fail miscdata_path = %s ", __FUNCTION__,
              miscdata_path);
    }
    fsync(fd);
    close(fd);
  } else {
    ENG_LOG("%s open fail miscdata_path = %s ", __FUNCTION__,
            miscdata_path);
    ret = -3;
  }
#endif

  return ret;
}

unsigned long get_file_size(const char *path)
{
/*
	unsigned long filesize = -1;
	struct stat statbuff;
	if(stat(path, &statbuff) < 0){
		return filesize;
	}else{
		filesize = statbuff.st_size;
	}
	return filesize;
	*/
	return NAND_MISCDATA_FILESIZE;
}

int eng_diag_product_ctrl(char *buf, int len, char *rsp, int rsplen) {
  int offset = 0;
  int data_len = 0;
  int head_len = 0;
  int rsp_len = 0;
  unsigned char *nvdata = NULL;
  NVITEM_ERROR_E nverr = NVERR_NONE;
  MSG_HEAD_T *msg_head = (MSG_HEAD_T *)(buf + 1);

  head_len = sizeof(MSG_HEAD_T) + 2 * sizeof(unsigned short);
  offset = *(unsigned short *)((char *)msg_head + sizeof(MSG_HEAD_T));
  data_len = *(unsigned short *)((char *)msg_head + sizeof(MSG_HEAD_T) +
                                 sizeof(unsigned short));

  ENG_LOG("%s: offset: %d, data_len: %d\n", __FUNCTION__, offset, data_len);

  if (rsplen < (head_len + data_len + 2)) {  // 2:0x7e
    ENG_LOG("%s: Rsp buffer is not enough, need buf: %d\n", __FUNCTION__,
            head_len + data_len);
    return 0;
  }

  // 2: NVITEM_PRODUCT_CTRL_READ
  // 3: NVITEM_PRODUCT_CTRL_WRITE
  ENG_LOG("%s: msg_head->subtype: %d\n", __FUNCTION__, msg_head->subtype);
  switch (msg_head->subtype) {
    case 2: {
      nvdata = (unsigned char *)malloc(data_len + head_len);
      memcpy(nvdata, msg_head, head_len);

      nverr = eng_read_miscdata_with_offset(0, nvdata + head_len, data_len);
      if (NVERR_NONE != nverr) {
        ENG_LOG("%s: Read ERROR: %d\n", __FUNCTION__, nverr);
        data_len = 0;
      }

      ((MSG_HEAD_T *)nvdata)->subtype = nverr;
      ((MSG_HEAD_T *)nvdata)->len = head_len + data_len;

      rsp_len = translate_packet(rsp, nvdata, head_len + data_len);

      free(nvdata);
    } break;
    case 3: {
      nvdata = (unsigned char *)malloc(rsplen);

      nverr = eng_read_miscdata_with_offset(0, nvdata, data_len);
      if (NVERR_NONE != nverr) {
        ENG_LOG("%s: Read before writing ERROR: %d\n", __FUNCTION__, nverr);
      } else {
        memcpy(nvdata + offset, (char *)msg_head + head_len, data_len);
        nverr = eng_write_miscdata_with_offset(0, nvdata, data_len);
        if (NVERR_NONE != nverr) {
          ENG_LOG("%s:Write ERROR: %d\n", __FUNCTION__, nverr);
        }
      }

      free(nvdata);

      msg_head->subtype = nverr;
      msg_head->len = sizeof(MSG_HEAD_T);

      rsp_len =
          translate_packet(rsp, (unsigned char *)msg_head, sizeof(MSG_HEAD_T));
    } break;
    default:
      ENG_LOG("%s: ERROR Oper: %d !\n", __FUNCTION__, msg_head->subtype);
      return 0;
  }

  ENG_LOG("%s: rsp_len : %d\n", __FUNCTION__, rsp_len);

  return rsp_len;
}

int eng_diag_product_ctrl_ex(char *buf, int len, char *rsp, int rsplen) {
  int offset = 0;
  int data_len = 0;
  int head_len = 0;
  int rsp_len = 0;
  unsigned char *nvdata = NULL;
  NVITEM_ERROR_E nverr = NVERR_NONE;
  MSG_HEAD_T *msg_head = (MSG_HEAD_T *)(buf + 1);

  head_len = sizeof(MSG_HEAD_T) + 2 * sizeof(unsigned int);
  offset = *(unsigned int *)((char *)msg_head + sizeof(MSG_HEAD_T));
  data_len = *(unsigned int *)((char *)msg_head + sizeof(MSG_HEAD_T) +
                                 sizeof(unsigned int));

  ENG_LOG("%s: offset: %d, data_len: %d\n", __FUNCTION__, offset, data_len);

  if (rsplen < (head_len + data_len + 2)) {  // 2:0x7e
    ENG_LOG("%s: Rsp buffer is not enough, need buf: %d\n", __FUNCTION__,
            head_len + data_len);
    return 0;
  }

  // 12: NVITEM_PRODUCT_CTRL_READ_EX
  // 13: NVITEM_PRODUCT_CTRL_WRITE_EX
  ENG_LOG("%s: msg_head->subtype: %d\n", __FUNCTION__, msg_head->subtype);
  switch (msg_head->subtype) {
    case 0x12: {
      nvdata = (unsigned char *)malloc(data_len + head_len);
      memset(nvdata, 0, data_len + head_len);
      memcpy(nvdata, msg_head, head_len);

      nverr = eng_read_miscdata_with_offset(offset, nvdata + head_len, data_len);
      if (NVERR_NONE != nverr) {
        ENG_LOG("%s: Read ERROR: %d\n", __FUNCTION__, nverr);
        data_len = 0;
      }

      ((MSG_HEAD_T *)nvdata)->subtype = nverr;
      ((MSG_HEAD_T *)nvdata)->len = head_len + data_len;

      rsp_len = translate_packet(rsp, nvdata, head_len + data_len);

      free(nvdata);
    } break;
    case 0x13: {
      nvdata = (unsigned char *)malloc(data_len);
      memset(nvdata, 0, data_len);
      nverr = eng_read_miscdata_with_offset(offset, nvdata, data_len);
      if (NVERR_NONE != nverr) {
        ENG_LOG("%s: Read before writing ERROR: %d\n", __FUNCTION__, nverr);
      } else {
        memcpy(nvdata, (char *)msg_head + head_len, data_len);
        nverr = eng_write_miscdata_with_offset(offset, nvdata, data_len);
        if (NVERR_NONE != nverr) {
          ENG_LOG("%s:Write ERROR: %d\n", __FUNCTION__, nverr);
        }
      }

      free(nvdata);

      msg_head->subtype = nverr;
      msg_head->len = sizeof(MSG_HEAD_T);

      rsp_len =
          translate_packet(rsp, (unsigned char *)msg_head, sizeof(MSG_HEAD_T));
    } break;
    default:
      ENG_LOG("%s: ERROR Oper: %d !\n", __FUNCTION__, msg_head->subtype);
      return 0;
  }

  ENG_LOG("%s: rsp_len : %d\n", __FUNCTION__, rsp_len);

  return rsp_len;
}