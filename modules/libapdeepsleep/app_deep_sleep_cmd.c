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

#define SYS_CLASS_ANDUSB_STATE "/sys/class/android_usb/android0/state"
#define MODEM_MAGIC 'M'
#define MODEM_ENTER_SLEEP_CMD _IO(MODEM_MAGIC, 0x12)

static int wakeup_count_fd = -1;

static pthread_t suspend_thread;
static const char sys_power_wakeup_count[] = "/sys/power/wakeup_count";

int ap_ds_usb_state(void) {
  int fd = -1;
  int ret = 0;
  char usb_state[32] = {0};

  fd = open(SYS_CLASS_ANDUSB_STATE, O_RDONLY);
  if (fd >= 0) {
    ret = read(fd, usb_state, 32);
    if (ret > 0) {
      if (0 == strncmp(usb_state, "CONFIGURED", 10)) {
        ret = 1;
      } else {
        //ENG_LOG("%s: usb state: %s\n", __FUNCTION__, usb_state);
        ret = 0;
      }
    } else {
      ret = 0;
      //ENG_LOG("%s: Read sys class androidusb state file failed, read:%d\n",
              //__FUNCTION__, ret);
    }

    close(fd);
  } else {
    ret = 0;
    //ENG_LOG("%s: Open sys class androidusb state file failed, err: %s!\n",
            //__FUNCTION__, strerror(errno));
  }

  return ret;
}

static void eng_modem_enter_sleep(void) {
  int ret, fd;

  ENG_LOG("eng_modem_enter_sleep");

  fd = open("/dev/modem", O_RDWR);
  if (fd < 0) {
    ENG_LOG("open modem dev error(%s)", strerror(errno));
    return;
  }

  ret = ioctl(fd, MODEM_ENTER_SLEEP_CMD, NULL);
  if (ret)
    ENG_LOG("ioctl modem error(%s)", strerror(errno));

  close(fd);
}

void *ap_ds_thread_fastsleep(void *para) {

  int count;
  ssize_t ret;
  int sleep = 0;
  char cmd[] = {"echo mem > /sys/power/state"};
  char wakeup_count[32];

  eng_modem_enter_sleep();

  ENG_LOG("[apdeepsleep]##: please plug out usb within 60s...\n");
  for (count = 0; count < 60*5; count ++) {
    if (!ap_ds_usb_state()){
      sleep = 1;
      break;
    }
    usleep(200*1000);
  }
  ENG_LOG("[apdeepsleep]##: sleep count=%d\n", count);
  while (sleep) {
    ENG_LOG("[apdeepsleep]##: going to sleep mode!delay 3s\n");
    usleep(3000*1000);
	lseek(wakeup_count_fd, 0, SEEK_SET);

	ret = read(wakeup_count_fd, &wakeup_count[0], sizeof(wakeup_count));
	if (ret > 0)
		ENG_LOG("%s read wakeup count success %s.\n", __FUNCTION__, wakeup_count);
	else {
		ENG_LOG("%s read wakeup count failed, ret = %d.\n", __FUNCTION__, ret);
		continue;
	}

	ret = write(wakeup_count_fd, wakeup_count, sizeof(wakeup_count));
	if (ret) {
		ENG_LOG("%s enter suspend.\n", __FUNCTION__);
		system(cmd);
	}
	else {
		ENG_LOG("%s write wakeup count failed.\n", __FUNCTION__);
		continue;
	}
	ENG_LOG("%s suspend success.\n", __FUNCTION__);
  }

  return NULL;
}

// return : rsp true length
int ap_deep_sleep_handler(char *buf, int len, char *rsp, int rsplen){
  int ret;

  wakeup_count_fd = open(sys_power_wakeup_count, O_RDWR);
  if(wakeup_count_fd < 0) {
	  ENG_LOG("%s error open wakeup_count.\n", __FUNCTION__);
	  goto err_open_wakeup_count;
  }
  ENG_LOG("%s init wakeup_count success.\n", __FUNCTION__);

  ret = pthread_create(&suspend_thread, NULL, ap_ds_thread_fastsleep, NULL);
  if (0 != ret) {
    ENG_LOG("%s: Can't create thread[thread_fastsleep]!\n", __FUNCTION__);
	goto err_pthread_create;
  }
   return 2;

err_pthread_create:
   close(wakeup_count_fd);
err_open_wakeup_count:
   return -1;
}

void register_this_module_ext(struct eng_callback *reg, int *num)
{
    int moudles_num = 0;
    //ENG_LOG("register_this_module_ext :libapdeepsleep");

    //1st command
    reg->type = 0x11; //main cmd
    reg->subtype = 0x2; //sub cmd
    reg->also_need_to_cp = 1;  //deep sleep cmd is also dealed with upon cp side
    reg->eng_diag_func = ap_deep_sleep_handler; // rsp function ptr
    moudles_num++;

    *num = moudles_num;
    //ENG_LOG("register_this_module_ext: %d - %d",*num, moudles_num);
}
