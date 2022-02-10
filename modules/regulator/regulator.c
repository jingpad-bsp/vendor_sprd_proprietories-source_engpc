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

#define VDDWCN_VOLTAGE "/sys/kernel/debug/LDO_VDDWCN/voltage"

typedef struct {
	unsigned short cmd;
	unsigned short length;
	unsigned int pmu_action;
} ENGPC_REGULATOR_REQ_CMD;

typedef struct {
	unsigned short status;
	unsigned short length;
	unsigned int voltage;
} ENGPC_REGULATOR_CFG_RSP;

static int regulator_set_voltage(unsigned int vol)
{
	int ret, fd = -1;
	char voltage[5];

	fd = open(VDDWCN_VOLTAGE, O_RDWR);
	if (fd < 0) {
		ENG_LOG("%s error open vddwcn voltage.\n", __FUNCTION__);
		return -1;
	}
	ENG_LOG("%s open vddwcn voltage success.\n", __FUNCTION__);

	sprintf(voltage, "%d", vol);

	ret = write(fd, voltage, sizeof(voltage));
	if (ret < 0) {
		ENG_LOG("%s set vddwcn voltage failed.\n", __FUNCTION__);
		close(fd);
		return -1;
	}

	close(fd);

	ENG_LOG("%s, set vddwcn voltage: %d.\n", __FUNCTION__, vol);
	return 0;
}

static unsigned int regulator_get_voltage()
{
	int fd = -1;
	int read_len;
	unsigned int vol = 0;
	char buffer[32] = {0};

	fd = open(VDDWCN_VOLTAGE, O_RDWR);
	if (fd < 0) {
		ENG_LOG("%s error open vddwcn voltage.\n", __FUNCTION__);
		return 0;
	}
	ENG_LOG("%s open vddwcn voltage success.\n", __FUNCTION__);

	read_len = read(fd, buffer, sizeof(buffer));
	if (read_len > 0)
		vol = atoi(buffer);

	close(fd);

	ENG_LOG("%s, return vddwcn voltage: %d.\n", __FUNCTION__, vol);
	return vol;
}

/*
   In TX_IRR calibration mode, it sets vddwcn voltage to 1.0V and then
   restore this voltage after calibration.
*/
static int regulator_cali_handler(char *buf, int len, char *rsp, int rsplen)
{
	int length, ret = -1;
	unsigned int voltage = 0;
	ENGPC_REGULATOR_REQ_CMD *regu_ctl_cmd;
	ENGPC_REGULATOR_CFG_RSP *regu_cfg_rsp;

	if (NULL == buf) {
		ENG_LOG("%s,null pointer", __FUNCTION__);
		return 0;
	}

	memcpy(rsp, buf, 1 + sizeof(MSG_HEAD_T));

	regu_ctl_cmd = (ENGPC_REGULATOR_REQ_CMD *)(buf + 1 + sizeof(MSG_HEAD_T));

	regu_cfg_rsp = (ENGPC_REGULATOR_CFG_RSP *)(rsp + 1 + sizeof(MSG_HEAD_T));
	regu_cfg_rsp->length = sizeof(unsigned int);

	if (regu_ctl_cmd->pmu_action)
		ret = regulator_set_voltage(1000);
	else
		ret = regulator_set_voltage(900);

	voltage = regulator_get_voltage();
	if (voltage && !ret) {
		regu_cfg_rsp->status = 0x00;
		regu_cfg_rsp->voltage = voltage;
		ENG_LOG("%s, vddwcn voltage: %d, ret: %d, success", __FUNCTION__, voltage, ret);
	} else {
		regu_cfg_rsp->status = 0x01;
		regu_cfg_rsp->voltage = 0;
		ENG_LOG("%s, vddwcn voltage: %d, ret: %d, failed", __FUNCTION__, voltage, ret);
	}

	length = sizeof(MSG_HEAD_T) + sizeof(ENGPC_REGULATOR_CFG_RSP);
	rsp[5] = length;
	rsp[length + 1] = 0x7E;

	return length + 2;
}

void register_this_module_ext(struct eng_callback *reg, int *num)
{
	int moudles_num = 0;

	ENG_LOG("register_this_module_ext: regulator_cali\n");

	reg->type = 0x62;
	reg->subtype = 0x0;
	reg->diag_ap_cmd = 0x80;
	reg->eng_diag_func = regulator_cali_handler;

	moudles_num++;
	*num = moudles_num;

	ENG_LOG("register_this_module_ext: %d %d", *num, moudles_num);
}
