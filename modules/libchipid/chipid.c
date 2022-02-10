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
#include <sys/types.h>

#include "sprd_fts_type.h"
#include "sprd_fts_log.h"
#include "sprd_fts_diag.h"


static int pmic_chipid_get(char *req, char *rsp)
{
    int fd = 0;
    unsigned int sizes;
    char *pchar;
    char *ptemp;
    char cmdline[4096] = {0};
    char rsptemp[10] = {0};

    fd = open("/proc/cmdline", O_RDONLY);
    if (fd < 0) {
	    ENG_LOG("%s: open file failed, err: %s\n", __func__,
		    strerror(errno));
	    return errno;
    }

    ENG_LOG("libchipid :enter function test :AT+CHIPID=PMIC?");
    sizes = read(fd, cmdline, sizeof(cmdline) - 1);
    pchar = strstr(cmdline, "androidboot.pmic.chipid");
    if (pchar == NULL) {
	sprintf(rsp,"%s", "Not Find");
	return 0;
    }

    pchar = strstr(pchar, "=");
    ptemp = strchr(pchar + 1, ' ');
    sizes = ptemp - pchar - 1;
    strncpy(rsptemp, pchar + 1, sizes);
    sprintf(rsp,"%s%s","PMIC:",rsptemp);
    ENG_LOG("AT+CHIPID=PMIC is %s",rsp);
    close(fd);

    return 0;
}

static int hw_chipid_get(char *req, char *rsp)
{
    int fd = 0;
    unsigned int sizes;
    char *pchar;
    char *ptemp;
    char *ptail;
    char cmdline[4096] = {0};
    char rsptemp[10] = {0};

    fd = open("/proc/cmdline", O_RDONLY);
    if (fd < 0) {
	    ENG_LOG("%s: open file failed, err: %s\n", __func__,
		    strerror(errno));
	    return errno;
    }

    ENG_LOG("libchipid :enter function test :AT+CHIPID=BB?");
    sizes = read(fd, cmdline, sizeof(cmdline) - 1);
    pchar = strstr(cmdline, "androidboot.hardware");
    if (pchar == NULL) {
	sprintf(rsp,"%s", "Not Find");
	return 0;
    }

    pchar = strstr(pchar, "=");
    ptemp = strchr(pchar + 1, '_');
    ptail = strchr(pchar + 1, ' ');
    if (ptail < ptemp || !ptemp)
	sizes = ptail - pchar - 1;
    else
	sizes = ptemp - pchar - 1;
    strncpy(rsptemp, pchar + 1, sizes);
    sprintf(rsp,"%s%s","BB:",rsptemp);
    ENG_LOG("AT+CHIPID=BB is %s",rsp);
    close(fd);

    return 0;
}
void register_this_module_ext(struct eng_callback *reg, int *num)
{
    int moudles_num = 0;
    ENG_LOG("register_this_module_ext :libchipid");

    //1st command
    sprintf(reg->at_cmd, "%s", "AT+CHIPID=PMIC?");
    reg->eng_linuxcmd_func = pmic_chipid_get; // rsp function ptr
    moudles_num++;
    reg++;

    //2st command
    sprintf(reg->at_cmd, "%s", "AT+CHIPID=BB?");
    reg->eng_linuxcmd_func = hw_chipid_get; // rsp function ptr
    moudles_num++;
    reg++;

    *num = moudles_num;
    ENG_LOG("register_this_module_ext: %d - %d",*num, moudles_num);
}
