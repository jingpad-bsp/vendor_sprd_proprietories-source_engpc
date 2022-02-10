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
#include <sys/wait.h>
#include <log/log.h>

#include "sprd_fts_diag.h"

#define ENG_LOG(fmt, args...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, fmt, ##args)

typedef enum {
  ACTION_IDLE = 0,
  ACTION_TARRING = 1,
  ACTION_TARRED = 2,
  ACTION_DUMPING = 3,
  ACTION_DUMPED = 4,
  ACTION_MAX = 0xff
} ACTION_STATUS;

typedef enum {
  DUMP_TRACE_CMD_START,
  DUMP_UBOOT_TRACE = 1,   // 1: Dump UBOOT log
  DUMP_YLOG_TRACE = 2,  // 1: Dump ylog
  DUMP_TRACE_CMD_END
} DUMP_TRACE_CMD;

typedef struct {
  unsigned int cmd; // 1: Dump UBOOT log   2: Dump ylog
  unsigned int reserved; // 保留扩展用
} DIAG_TRACE_DUMP_REQ_T;

typedef struct {
  unsigned int cmd; // 1: Dump UBOOT log   2: Dump ylog
  unsigned int status; // 0: 传输结束  1: 传输未完成，     其他 为传输错误具体代码
  unsigned int data_len; // pData的数据大小，单位字节
  unsigned char *pData;    // 实际DUMP的数据
} DIAG_TRACE_DUMP_ACK_T;

#define TRACE_FILE "/mnt/vendor/trace_dump_test.txt"
#define YLOG_FILE "/cache/ylog.tar.gz"

#define TRACE_DUMP_UNIT_SIZE (4 * 1024)

static ACTION_STATUS dump_action = ACTION_IDLE;
static int dump_fd = -1;
static int dump_offset = 0;

int translate_packet(char *dest, char *src, int size) {
  int i;
  int translated_size = 0;

  dest[translated_size++] = 0x7E;

  for (i = 0; i < size; i++) {
    if (src[i] == 0x7E) {
      dest[translated_size++] = 0x7D;
      dest[translated_size++] = 0x5E;
    } else if (src[i] == 0x7D) {
      dest[translated_size++] = 0x7D;
      dest[translated_size++] = 0x5D;
    } else
      dest[translated_size++] = src[i];
  }
  dest[translated_size++] = 0x7E;
  return translated_size;
}

//flag 1: print once; 0: print all
void eng_dump(unsigned char *buf, int len, int col, int flag, char *keyword)
{
    int i = 0;
    int j = 0;
    unsigned char tmpbuf[128] = {0};

    for(i = 0; i < len; i++) {
        sprintf(tmpbuf + j, "%02X ", *(buf + i));
        j = strlen(tmpbuf);
        if(i % col == col -1 || i == len -1) {
            ENG_LOG("%s %s len=%d buf: %s\n", keyword, __FUNCTION__, len, tmpbuf);
            if (flag) {
                break;
            } else {
                j = 0;
                continue;
            }
        }
    }
    return;
}

int trace_dump_read(unsigned char *buf, int fd, int offset) {
  int count = 0;

  count = read(fd, buf, TRACE_DUMP_UNIT_SIZE);
  ENG_LOG("%s count = %d", __FUNCTION__, count);
  if (count <= 0) {
    return 0;
  }

  return count;
}

// return : rsp true length
int trace_dump(char *buf, int len, char *rsp, int rsplen) {
  char *rsp_ptr;
  MSG_HEAD_T *msg_head_ptr;
  DIAG_TRACE_DUMP_REQ_T *dump_trace_req_ptr = NULL;
  DIAG_TRACE_DUMP_ACK_T *dump_trace_ack_ptr = NULL;
  DUMP_TRACE_CMD cmd_type = DUMP_TRACE_CMD_START;
  unsigned char *readbuf = NULL;
  int count = 0;
  int ret = 0;
  char cmd[128] = {0};

  dump_trace_req_ptr = (DIAG_TRACE_DUMP_REQ_T *)(buf + 1 + sizeof(MSG_HEAD_T));
  cmd_type = dump_trace_req_ptr->cmd;

  ENG_LOG("%s cmd_type = %d, dump_action = %d", __FUNCTION__, cmd_type, dump_action);

  if (cmd_type <= DUMP_TRACE_CMD_START || cmd_type >= DUMP_TRACE_CMD_END) {
    ENG_LOG("%s cmd_type:%d is invalid!\n", __FUNCTION__);
    return 0;
  }

  if (dump_action == ACTION_IDLE) {
    if (cmd_type == DUMP_YLOG_TRACE) {
      ret = system("rm -rf /cache/ylog.tar.gz");
      memset(cmd, 0, sizeof(cmd));
      sprintf(cmd, "tar zcvf %s /cache/ylog/", YLOG_FILE);
      ret = system(cmd);
      if (!WIFEXITED(ret) || WEXITSTATUS(ret) || -1 == ret) {
        ENG_LOG("system[tar zcvf /cache/ylog.tar.gz /cache/ylog/] failed.");
        return 0;
      }
      system("sync");
    }

    if (cmd_type == DUMP_YLOG_TRACE) {
      dump_fd = open(YLOG_FILE, O_RDONLY);
    } else {
      dump_fd = open(TRACE_FILE, O_RDONLY);
    }

    if (dump_fd < 0) {
      ENG_LOG("%s: open fail errno=%d, strerror(errno)=%s", __FUNCTION__, errno,
              strerror(errno));
      return 0;
    }
    dump_action = ACTION_DUMPING;
  }

  if (dump_action == ACTION_DUMPING) {
    rsp_ptr = (char *)malloc(sizeof(MSG_HEAD_T) + sizeof(DIAG_TRACE_DUMP_ACK_T) + TRACE_DUMP_UNIT_SIZE);
    if (NULL == rsp_ptr) {
      ENG_LOG("%s: rsp_ptr buffer malloc failed\n", __FUNCTION__);
      dump_action = ACTION_IDLE;
      close(dump_fd);
      return 0;
    }
    msg_head_ptr = (MSG_HEAD_T *)(buf + 1);
    memcpy(rsp_ptr, msg_head_ptr, sizeof(MSG_HEAD_T));
    msg_head_ptr = (MSG_HEAD_T *)(rsp_ptr);

    dump_trace_ack_ptr = (DIAG_TRACE_DUMP_ACK_T *)(rsp_ptr + sizeof(MSG_HEAD_T));

    dump_trace_ack_ptr->cmd = cmd_type;

    readbuf = (unsigned char *)malloc(TRACE_DUMP_UNIT_SIZE);
    if (NULL != readbuf) {
      memset(readbuf, 0, TRACE_DUMP_UNIT_SIZE);
      count = trace_dump_read(readbuf, dump_fd, dump_offset);
      dump_offset = count;
      //eng_dump(readbuf, count, 40, 0, __FUNCTION__);
      memcpy(rsp_ptr + sizeof(MSG_HEAD_T) + sizeof(DIAG_TRACE_DUMP_ACK_T) - sizeof(char *), readbuf, count);
      dump_trace_ack_ptr->data_len = count;
      if (count < TRACE_DUMP_UNIT_SIZE) {
        dump_trace_ack_ptr->status = 0;
        dump_action = ACTION_IDLE;
        close(dump_fd);
      } else {
        dump_trace_ack_ptr->status = 1;
      }
    } else {
      ENG_LOG("%s malloc failed!", __FUNCTION__);
      dump_trace_ack_ptr->data_len = 0;
      dump_trace_ack_ptr->status = 2; // malloc failed
      dump_action = ACTION_IDLE;
      close(dump_fd);
    }

    msg_head_ptr->len = sizeof(MSG_HEAD_T) + sizeof(DIAG_TRACE_DUMP_ACK_T) + dump_trace_ack_ptr->data_len - 1;
    if (NULL != readbuf) {
      free(readbuf);
      readbuf = NULL;
    }
    rsplen = translate_packet(rsp, (unsigned char *)rsp_ptr,
                            ((MSG_HEAD_T *)rsp_ptr)->len);
    ENG_LOG("%s :return len:%d", __FUNCTION__, rsplen);
    free(rsp_ptr);
    //eng_dump(rsp, msg_head_ptr->len + 2, 40, 0, __FUNCTION__);
    return rsplen;
  }

  return 0;
}

/*
void register_this_module_ext(struct eng_callback *reg, int *num) {
  int moudles_num = 0;
  ENG_LOG("register_this_module_ext :libreadfixednv");

  // Dump UBOOT log
  reg->type = 0x5D;   // main cmd
  reg->subtype = 0x4; // sub cmd
  reg->diag_ap_cmd = 0x1;
  reg->eng_diag_func = trace_dump; // rsp function ptr
  moudles_num++;

  // Dump ylog
  (reg + 1)->type = 0x5D;   // main cmd
  (reg + 1)->subtype = 0x4; // sub cmd
  (reg + 1)->diag_ap_cmd = 0x2;
  (reg + 1)->eng_diag_func = trace_dump; // rsp function ptr
  moudles_num++;

  *num = moudles_num;
  dump_action = ACTION_IDLE;

  ENG_LOG("register_this_module_ext: %d - %d", *num, moudles_num);
}
*/
