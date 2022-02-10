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
#include <unistd.h>

#include "translate_packet.h"
#include "mmiresult.h"

static int parse_string(char * buf, char gap, char* value)
{
  int len = 0;
  char *ch = NULL;
  char str[10] = {0};

  if(buf != NULL && value  != NULL){
    ch = strchr(buf, gap);
    if(ch != NULL){
      len = ch - buf ;
      strncpy(str, buf, len);
      *value = atoi(str);
    }
  }
  return len;
}

static int parse_2_entries(char *type, char* arg1, char* arg2)
{
  int len;
  char *str = type;

  /* sanity check */
  if(str == NULL) {
    ENG_LOG("type is null!");
    return -1;
  }

  if((len = parse_string(str, '\t', arg1)) <= 0)  return -1;
  str += len + 1;
  if(str == NULL) {
    ENG_LOG("mmitest type is null!");
    return -1;
  }
  if((len = parse_string(str, '\t', arg2)) <= 0)  return -1;

  return 0;
}

static int parse_config(void)
{
  FILE *fp;
  int ret = 0, count = 0;
  char id,flag;
  char buffer[MAX_LINE_LEN]={0};

  /*for BBAT*/
  fp = fopen(ENG_BBAT_SUPPORT_CONFIG, "r");
  if(fp == NULL) {
    ENG_LOG("bbattest open %s failed, %s", ENG_BBAT_SUPPORT_CONFIG,strerror(errno));
    return -1;
  }

  /* parse line by line */
  ret = 0;
  count = 0;
  while(fgets(buffer, MAX_LINE_LEN, fp) != NULL) {
    if(buffer[0] == '#')
      continue;
    if((buffer[0]>='0') && (buffer[0]<='9')){
      ret = parse_2_entries(buffer,&id,&flag);
      if(ret != 0) {
        ENG_LOG("bbattest parse %s return %d.  reload", ENG_BBAT_SUPPORT_CONFIG,ret);
        fclose(fp);
        return -1;
      }
      bbat_support_result[count].id = id;
      bbat_support_result[count++].support= flag;
    }
  }

  fclose(fp);
  if(count < MAX_BBAT_ITEM_COUNT) {
    ENG_LOG("bbattest parse BBAT.conf failed");
  }

  return ret;
}

int eng_init_test_file(void)
{
  int i,len = 0;
  int fd_bbat = -1;
  TEST_NEW_RESULT_INFO result[MAX_BBAT_ITEM_COUNT] = {0};
  memset(bbat_support_result, 0, sizeof(bbat_support_result));
  parse_config();

  if (0 != access(BBAT_TEST_FILE_PATH,F_OK)){
    fd_bbat = open(BBAT_TEST_FILE_PATH,O_RDWR|O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO);
    if (fd_bbat < 0 && (errno != EEXIST)) {
      ENG_LOG("%s,creat %s failed.",__FUNCTION__,BBAT_TEST_FILE_PATH);
      return 0;
    }
    for(i = 0;i < MAX_BBAT_ITEM_COUNT; i++){
      result[i].type_id = 2;
      result[i].function_id = i;
      result[i].support= bbat_support_result[i].support;
      result[i].status = 0;
    }
    len = write(fd_bbat,result,sizeof(result));
    if(len < 0){
      ENG_LOG("%s %s write_len = %d,type = %d\n",__FUNCTION__,BBAT_TEST_FILE_PATH,len);
    } else {
      fsync(fd_bbat);
    }
  }

  if(fd_bbat >= 0)
    close(fd_bbat);

  return 1;
}

int eng_diag_read_mmi(char *buf, int len, char *rsp, int rsplen)
{
  int fd = -1;
  int read_len;
  char buffer[256];
  char *rsp_ptr;
  MSG_HEAD_T* msg_head_ptr;
  TOOLS_DIAG_AP_CNF_T* aprsp;
  TOOLS_DIAG_MMI_CIT_T* test_result,*req_ptr;
  ENG_LOG("%s start",__FUNCTION__);
  if(NULL == buf){
    ENG_LOG("%s,null pointer",__FUNCTION__);
    return 0;
  }

  msg_head_ptr = (MSG_HEAD_T*)(buf + 1);
  req_ptr = (TOOLS_DIAG_MMI_CIT_T*)(buf + 1 + sizeof(MSG_HEAD_T)+sizeof(TOOLS_DIAG_AP_CMD_T));
  rsplen = sizeof(TOOLS_DIAG_AP_CNF_T)+ sizeof(TOOLS_DIAG_MMI_CIT_T) + sizeof(MSG_HEAD_T);
  rsp_ptr = (char*)malloc(rsplen);
  if(NULL == rsp_ptr){
    ENG_LOG("%s: Buffer malloc failed\n", __FUNCTION__);
    return 0;
  }
  aprsp = (TOOLS_DIAG_AP_CNF_T*)(rsp_ptr + sizeof(MSG_HEAD_T));
  test_result = (TOOLS_DIAG_MMI_CIT_T*)(rsp_ptr + sizeof(MSG_HEAD_T) + sizeof(TOOLS_DIAG_AP_CNF_T));
  memcpy(rsp_ptr,msg_head_ptr,sizeof(MSG_HEAD_T));
  ((MSG_HEAD_T*)rsp_ptr)->len = rsplen;
  aprsp->status = 0x01;
  aprsp->length= sizeof(TOOLS_DIAG_MMI_CIT_T);

  if( 0 == req_ptr->uType){
    fd = open(WHOLE_PHONE_TEST_FILE_PATH,O_RDONLY); //00: whole phone test; 01: PCBA test ; 02: BBAT test
  }else if(1 == req_ptr->uType){
    fd = open(PCBA_TEST_FILE_PATH,O_RDONLY);
  }else if(2 == req_ptr->uType){
    fd = open(BBAT_TEST_FILE_PATH,O_RDONLY);
  }
  if(fd < 0){
    ENG_LOG("%s open failed,type = %d\n",__FUNCTION__,req_ptr->uType);
    goto out;
  }
  read_len = read(fd,buffer,sizeof(buffer));
  if(read_len < 0){
    ENG_LOG("%s read failed! read_len = %d,type = %d\n",__FUNCTION__,read_len,req_ptr->uType);
    goto out;
  }

  memcpy(test_result->uBuff,buffer,read_len);
  aprsp->status = 0x00;

out:
  rsplen = translate_packet(rsp,(unsigned char*)rsp_ptr,((MSG_HEAD_T*)rsp_ptr)->len);
  free(rsp_ptr);
  if(fd >= 0 )
    close(fd);
  return rsplen;
}

int eng_diag_write_mmi(char *buf, int len, char *rsp, int rsplen)
{
  int fd = -1;
  int write_len;
  char *rsp_ptr;
  MSG_HEAD_T* msg_head_ptr;
  TOOLS_DIAG_AP_CNF_T* aprsp;
  TOOLS_DIAG_MMI_CIT_T* req_ptr;

  if(NULL == buf){
    ENG_LOG("%s,null pointer",__FUNCTION__);
    return 0;
  }

  msg_head_ptr = (MSG_HEAD_T*)(buf + 1);
  req_ptr = (TOOLS_DIAG_MMI_CIT_T*)(buf + 1 + sizeof(MSG_HEAD_T)+sizeof(TOOLS_DIAG_AP_CMD_T));

  rsplen = sizeof(TOOLS_DIAG_AP_CNF_T) + sizeof(MSG_HEAD_T);
  rsp_ptr = (char*)malloc(rsplen);
  if(NULL == rsp_ptr){
    ENG_LOG("%s: Buffer malloc failed\n", __FUNCTION__);
    return 0;
  }
  aprsp = (TOOLS_DIAG_AP_CNF_T*)(rsp_ptr + sizeof(MSG_HEAD_T));
  memcpy(rsp_ptr,msg_head_ptr,sizeof(MSG_HEAD_T));
  ((MSG_HEAD_T*)rsp_ptr)->len = rsplen;
  aprsp->status = 0x01;
  aprsp->length= 0;

  if( 0 == req_ptr->uType){
    fd = open(WHOLE_PHONE_TEST_FILE_PATH,O_WRONLY); //00: whole phone test; 01: PCBA test ; 02: BBAT test
    //fd = open(WHOLE_PHONE_TEST_FILE_PATH,O_CREAT | O_RDWR | O_TRUNC, 0666);
  }else if(1 == req_ptr->uType){
    fd = open(PCBA_TEST_FILE_PATH,O_WRONLY);
    //fd = open(PCBA_TEST_FILE_PATH,O_CREAT | O_RDWR | O_TRUNC, 0666);
  }else if(2 == req_ptr->uType){
    fd = open(BBAT_TEST_FILE_PATH,O_WRONLY);
    //fd = open(BBAT_TEST_FILE_PATH,O_CREAT | O_RDWR | O_TRUNC, 0666);
  }
  if(fd < 0){
    ENG_LOG("%s open failed,type = %d\n",__FUNCTION__,req_ptr->uType);
    goto out;
  }
  write_len = write(fd,req_ptr->uBuff,sizeof(req_ptr->uBuff));
  if(write_len < 0){
    ENG_LOG("%s write failed!write_len = %d,type = %d\n",__FUNCTION__,write_len,req_ptr->uType);
    goto out;
  } else {
    fsync(fd);
  }

  aprsp->status = 0x00;

out:
  rsplen = translate_packet(rsp,(unsigned char*)rsp_ptr,((MSG_HEAD_T*)rsp_ptr)->len);
  free(rsp_ptr);
  if(fd >= 0 )
    close(fd);

  return rsplen;
}