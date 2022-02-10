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
#include "sprd_fts_diag.h"

#define TS_FW_VERSION_PATH	"/sys/touchscreen/firmware_version"
#define AT_GET_TS_FW	"AT+GETFWVER=tp"
#define TS_FW_VER_LENGTH	8

static int get_ts_firmware_version(char *req, char *rsp)
{
	int fd = 0;
	unsigned int sizes;
	char *pchar;

	char cmdline[128] = {0};
	char rsptemp[32] = {0};

	fd = open(TS_FW_VERSION_PATH, O_RDONLY);
	if (fd < 0) {
		ENG_LOG("%s: open %s failed, err: %s\n", __func__,
		    TS_FW_VERSION_PATH, strerror(errno));
		sprintf(rsp,"%s", "Open /sys/touchscreen/firmware_version failed.");
		return errno;
	}

	sizes = read(fd, cmdline, sizeof(cmdline) - 1);
	ENG_LOG("%s", cmdline);

	pchar = strstr(cmdline, "Read error!");
	if (pchar) {
		sprintf(rsp,"%s", "Please Resume the phone and try again!");
		return 0;
	}

	/* Style_1: 0x03 */
	pchar = strstr(cmdline, "0x");
	if(pchar){
		strncpy(rsptemp, pchar, TS_FW_VER_LENGTH);
		sprintf(rsp,"%s:%s",AT_GET_TS_FW,rsptemp);

		return 0;
	}

	/* Style_2: firmware version: 0x03 */
	pchar = strstr(cmdline, "firmware version:");
	if (pchar == NULL) {
		sprintf(rsp,"%s", "Read failed!");
		return 0;
	}else{
		pchar += sizeof("firmware version: ");
		strncpy(rsptemp, pchar, TS_FW_VER_LENGTH);
		sprintf(rsp,"%s:%s",AT_GET_TS_FW,rsptemp);
	}

	return 0;
}

void register_this_module_ext(struct eng_callback *reg, int *num)
{
	int moudles_num = 0;

	ENG_LOG("register_this_module_ext :libtouchscreen");
	sprintf(reg->at_cmd, "%s", AT_GET_TS_FW);
	reg->eng_linuxcmd_func = get_ts_firmware_version;
	moudles_num++;
	reg++;

	*num = moudles_num;
	ENG_LOG("register_this_module_ext: %d - %d",*num, moudles_num);
}
