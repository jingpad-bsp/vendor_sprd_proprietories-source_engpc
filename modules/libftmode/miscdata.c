#include <fcntl.h>
#include <pthread.h>

#ifdef CONFIG_NAND
#include <sys/ioctl.h>
#include <ubi-user.h>
#endif

#include <sys/stat.h>

#include "sprd_fts_type.h"
#include "sprd_fts_diag.h"
#include "sprd_fts_log.h"


#define NAND_MISCDATA_FILESIZE (1024*1024)

unsigned long get_file_size(const char *path);

int eng_read_miscdata(char *databuf, int data_len) {
  int ret = 0;
  int len;
  char prop[128] = {0};
  char miscdata_path[128] = {0};
  int fd = -1;

  if (-1 == property_get("ro.vendor.product.partitionpath", prop, "")){
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

  if (-1 == property_get("ro.vendor.product.partitionpath", prop, "")){
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

  if (-1 == property_get("ro.vendor.product.partitionpath", prop, "")){
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

  if (-1 == property_get("ro.vendor.product.partitionpath", prop, "")){
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