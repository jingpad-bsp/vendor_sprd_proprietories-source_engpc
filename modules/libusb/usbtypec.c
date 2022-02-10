#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include <getopt.h>
#include <ctype.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include "sprd_fts_type.h"
#include "sprd_fts_log.h"

#define USB_TYPEC_CC "/sys/class/typec/port0/typec_cc_polarity_role"
#define USB_TYPEC_SPEED "/sys/class/udc/20f00000.dwc3/current_speed"

int package_test_result_to_rsp(char *buf, char *rsp, int ret, char *data_to_pc) {
  MSG_HEAD_T *msg_head_ptr;
  memcpy(rsp, buf, 1 + sizeof(MSG_HEAD_T) - 1);
  msg_head_ptr = (MSG_HEAD_T *) (rsp + 1);
  msg_head_ptr->len = 8;
  ENG_LOG("msg_head_ptr,ret=%d", ret);
  if (ret < 0) {
    rsp[sizeof(MSG_HEAD_T)] = 1;
  } else if (ret == 0) {
    rsp[sizeof(MSG_HEAD_T)] = 0;
  } else if (ret > 0) {
    rsp[sizeof(MSG_HEAD_T)] = 0;
    memcpy(rsp + 1 + sizeof(MSG_HEAD_T), data_to_pc, 2);
    msg_head_ptr->len += 2;
  }
  rsp[msg_head_ptr->len + 2 - 1] = 0x7E;
  ENG_LOG("engpc->pc:%x %x %x %x %x %x %x %x %x %x %x %x", rsp[0], rsp[1],
          rsp[2], rsp[3], rsp[4], rsp[5], rsp[6], rsp[7], rsp[8], rsp[9],
          rsp[10], rsp[11]);
  return msg_head_ptr->len + 2;
}

int typec_get_cc_detect(char* cc_status) {
  char cc_string[5] = { 0 };
  int fd;
  int ret;

  fd = open(USB_TYPEC_CC, O_RDONLY, 0666);
  if (fd < 0) {
    ENG_LOG("open %s failed.", USB_TYPEC_CC);
    return -1;
  }
  ret = read(fd, cc_string, 4);
  if (ret < 0) {
    ENG_LOG("read %s failed.", USB_TYPEC_CC);
    close(fd);
    return -1;
  }
  close(fd);

  ENG_LOG("cc_string = %s", cc_string);

  if (!strncmp(cc_string, "cc_1", 4))
    *cc_status = 0x00;
  else if (!strncmp(cc_string, "cc_2", 4))
    *cc_status = 0x01;
  else
    *cc_status = 0x03;

  return ret;
}

int typec_get_speed_detect(char* speed_status) {
  char speed_string[12] = { 0 };
  int fd;
  int ret;

  fd = open(USB_TYPEC_SPEED, O_RDONLY, 0666);
  if (fd < 0) {
    ENG_LOG("open %s failed.", USB_TYPEC_SPEED);
    return -1;
  }

  ret = read(fd, speed_string, 12);
  if (ret < 0) {
    ENG_LOG("read %s failed.", USB_TYPEC_SPEED);
    close(fd);
    return -1;
  }
  close(fd);

  speed_string[ret - 1] = '\0';
  ENG_LOG("speed_string = %s", speed_string);

  if (!strncmp(speed_string, "high-speed", 10))
    *speed_status = 0x00;
  else if (!strncmp(speed_string, "super-speed", 11))
    *speed_status = 0x01;
  else
    *speed_status = 0x03;

  return ret;
}

int testTypeC(char *buf, int len, char *rsp, int rsplen) {
  char data_to_pc[2] = { 0 };
  int ret;
  int rsp_len;

  ENG_LOG("enter testTypeC!");

  ret = typec_get_cc_detect(&data_to_pc[0]);
  if (ret < 0) {
    ENG_LOG("typec_get_cc_detect failed.");
    goto exit;
  }

  ret = typec_get_speed_detect(&data_to_pc[1]);
  if (ret < 0) {
    ENG_LOG("typec_get_speed_detect failed.");
    goto exit;
  }

exit:
  rsp_len = package_test_result_to_rsp(buf, rsp, ret, data_to_pc);
  ENG_LOG("rsp_len = %d", rsp_len);

  return rsp_len;
}

void register_this_module_ext(struct eng_callback *reg, int *num) {
  ENG_LOG("register_this_module_ext :libusb");
  reg->type = 0x38;
  reg->subtype = 0x20;
  reg->eng_diag_func = testTypeC;
  *num = 1;
}
