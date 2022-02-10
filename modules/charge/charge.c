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

#define BATTERY_CAPACITY_PATH "/sys/class/power_supply/battery/capacity"
#define CHARGER_STOP_PATH "/sys/class/power_supply/battery/stop_charge"
#define CHARGER_STOP_KERNEL414_PATH "/sys/class/power_supply/battery/charger.0/stop_charge"
#define BATTERY_VOL_PATH "/sys/class/power_supply/battery/real_time_voltage"
#define FGU_VOL_FILE_PATH "/sys/class/power_supply/sprdfgu/fgu_vol"
#define BATTERY_FUEL_VOL "/sys/class/power_supply/battery/voltage_now"
#define BATTERY_TEMP "/sys/class/power_supply/battery/temp"
#define FGU_CURRENT_PATH "/sys/class/power_supply/battery/current_now"
#define CHARGER_INPUT_LIMIT_CURRENT "/sys/class/power_supply/battery/input_current_limit"
#define CHARGER_CURRENT "/sys/class/power_supply/battery/constant_charge_current"
#define CHARGER_INPUT_LIMIT_CURRENT_K44 "/sys/class/power_supply/battery/force_chg_cur"
#define CHARGER_CURRENT_K44 "/sys/class/power_supply/battery/force_input_cur"

#define ENG_DIAG_NTC_CMD        0x0a

typedef struct {
	unsigned short status;  //==0:success, != 0:fail
	unsigned short length;
} TOOLS_DIAG_AP_STATUS;

typedef struct {
	int capacity;
	int reserved;
} TOOLS_DIAG_AP_CAPACITY;

typedef struct {
	unsigned char status;
} TOOLS_DIAG_AP_CHARGE_STATUS;

typedef struct {
	unsigned char cmd;
} TOOLS_DIAG_AP_CHARGE_CMD;

typedef struct {
	unsigned short value;
	unsigned short res_cmd;
} TOOLS_DIAG_AP_AUX_VOLTAGE;

typedef struct {
	unsigned int value;
} TOOLS_DIAG_AP_FGU_VOLTAGE;

typedef struct {
	unsigned short stu;
	unsigned short value;
	unsigned int cmd;
} TOOLS_DIAG_AP_FGU_RESERVED;

typedef struct {
	unsigned char status;
} TOOLS_DIAG_AP_NTC_STATUS;

typedef struct {
	int temperature;
} TOOLS_DIAG_AP_NTC_TEMPERATURE;

typedef struct {
	unsigned short status;
	unsigned short length;
	unsigned int reserved;
	unsigned int current;
} TOOLS_DIAG_AP_CHG_CURRENT;

typedef struct {
	unsigned short cmd;
	unsigned short length;
	unsigned int current;
} ENGPC_CHARGER_CURRENT_CMD;

typedef struct {
	unsigned short status;
	unsigned short length;
} ENGPC_CHARGER_CURRENT_RSP;

typedef struct {
	unsigned short status;
	unsigned short length;
} ENGPC_CHARGER_SWITCH_STATUS_RSP;

static int connect_vbus_charger(void)
{
	int fd;
	int ret = -1;

	fd = open(CHARGER_STOP_PATH, O_WRONLY);
	if (fd >= 0) {
		ret = write(fd, "0", 2);
		if (ret < 0) {
			ENG_LOG("%s write %s failed! \n", __func__, CHARGER_STOP_PATH);
			close(fd);
			return 0;
		}
		close(fd);
		sleep(1);
	} else {
		fd = open(CHARGER_STOP_KERNEL414_PATH, O_WRONLY);
		if (fd >= 0) {
			ret = write(fd, "0", 2);
			if (ret < 0) {
				ENG_LOG("%s write %s failed! \n", __func__, CHARGER_STOP_KERNEL414_PATH);
				close(fd);
				return 0;
			}
			close(fd);
			sleep(1);
		}
	}
	ENG_LOG("connect_vbus_charger success.\n");

	return 1;
}

static int disconnect_vbus_charger(void)
{
	int fd;
	int ret = -1;

	fd = open(CHARGER_STOP_PATH, O_WRONLY);
	if (fd >= 0) {
		ret = write(fd, "1", 2);
		if (ret < 0) {
			ENG_LOG("%s write %s failed! \n", __func__, CHARGER_STOP_PATH);
			close(fd);
			return 0;
		}
		close(fd);
		sleep(1);
	} else {
		fd = open(CHARGER_STOP_KERNEL414_PATH, O_WRONLY);
		ENG_LOG("%s %d fd=%d\n", __func__, __LINE__, fd);
		if (fd >= 0) {
			ret = write(fd, "1", 2);
			if (ret < 0) {
				ENG_LOG("%s write %s failed! \n", __func__, CHARGER_STOP_KERNEL414_PATH);
				close(fd);
				return 0;
			}
			close(fd);
			sleep(1);
		}
	}
	ENG_LOG("disconnect_vbus_charger success.\n");

	return 1;
}

static int get_aux_battery_voltage(void)
{
	int fd = -1;
	int read_len = 0;
	char buffer[64] = {0};
	int value = 0;

	fd = open(BATTERY_VOL_PATH, O_RDONLY);
	ENG_LOG("get_aux_battery_voltage fd=%d\n", fd);

	if (fd >= 0) {
		read_len = read(fd, buffer, sizeof(buffer));
		if (read_len > 0) {
			value = atoi(buffer);
		}
		close(fd);
	} else {
		fd = open(BATTERY_FUEL_VOL, O_RDONLY);
		if (fd >= 0) {
			read_len = read(fd, buffer, sizeof(buffer));
			if (read_len > 0) {
				value = atoi(buffer);
				value = value / 1000;
			}
			close(fd);
		}
	}

	return value;
}

static int get_fgu_battery_voltage(void)
{
	int fd = -1;
	int read_len = 0;
	char buffer[64] = {0};
	int value = 0;

	fd = open(FGU_VOL_FILE_PATH, O_RDONLY);
	ENG_LOG("get_fgu_battery_voltage fd=%d\n", fd);

	if (fd >= 0) {
		read_len = read(fd, buffer, sizeof(buffer));
		if (read_len > 0) {
			value = atoi(buffer);
		}
		close(fd);
	} else {
		fd = open(BATTERY_FUEL_VOL, O_RDONLY);
		if (fd >= 0) {
			read_len = read(fd, buffer, sizeof(buffer));
			if (read_len > 0) {
				value = atoi(buffer);
				value = value / 1000;
			}
			close(fd);
		}
	}

	return value;
}

static int ap_get_aux_voltage(void)
{
	int voltage = 0;
	int i;

	for (i = 0; i < 16; i++) {
		voltage += get_aux_battery_voltage();
	}
	voltage >>= 4;

	return voltage;
}

static int ap_get_fgu_voltage(void)
{
	int voltage = 0;
	int i;

	for (i = 0; i < 16; i++) {
		voltage += get_fgu_battery_voltage();
	}
	voltage >>= 4;

	return voltage;
}

static int get_battery_temperature(void)
{
	int fd = -1;
	int read_len = 0;
	char buffer[64] = {0};
	int value = 0;

	fd = open(BATTERY_TEMP, O_RDONLY);
	ENG_LOG("get_battery_ntc_temperature fd=%d\n", fd);

	if (fd >= 0) {
		read_len = read(fd, buffer, sizeof(buffer));
		if (read_len > 0)
			value = atoi(buffer);

		close(fd);
	}
	ENG_LOG("get_battery_ntc_temperature value=%d\n", value);

	return value;
}

static int ap_read_ntc_temperature(void)
{
	int temp = 0;
	int i;

	for (i = 0; i < 16; i++) {
		temp += get_battery_temperature();
	}
	temp >>= 4;

	ENG_LOG("ap_read_ntc_temperature temp=%d\n", temp);
	return temp;
}

static int get_charge_current(void)
{
	int fd = -1;
	int read_len = 0;
	char buffer[64] = {0};
	int value = 0;

	fd = open(FGU_CURRENT_PATH, O_RDONLY);
	ENG_LOG("get_charge_current fd=%d\n", fd);

	if (fd >= 0) {
		read_len = read(fd, buffer, sizeof(buffer));
		if (read_len > 0) {
			value = atoi(buffer) / 1000;
			ENG_LOG("get_charge_current value=%d\n", value);
		}
	}
	close(fd);

	return value;
}

static int ap_get_charge_current(void)
{
	int temp = 0;
	int i;

	for (i = 0; i < 16; i++) {
		temp += get_charge_current();
	}
	temp >>= 4;

	ENG_LOG("ap_get_charge_current temp=%d\n", temp);
	return temp;
}

static int eng_diag_control_charge(char *buf, int len, char *rsp, int rsplen)
{
	int ret = 0;
	char *rsp_ptr;
	TOOLS_DIAG_AP_CHARGE_STATUS *aprsp;
	int length;

	if (NULL == buf) {
		ENG_LOG("%s,null pointer", __FUNCTION__);
		return 0;
	}
	MSG_HEAD_T *msg_head_ptr = (MSG_HEAD_T *)(buf + 1);
	TOOLS_DIAG_AP_CHARGE_CMD *charge =
		(TOOLS_DIAG_AP_CHARGE_CMD *)(buf + 1 + sizeof(MSG_HEAD_T));
	length = sizeof(TOOLS_DIAG_AP_CHARGE_CMD) + sizeof(MSG_HEAD_T);
	rsp_ptr = (char *)malloc(length);
	if (NULL == rsp_ptr) {
		ENG_LOG("%s: Buffer malloc failed\n", __FUNCTION__);
		return 0;
	}
	aprsp = (TOOLS_DIAG_AP_CHARGE_STATUS *)(rsp_ptr + sizeof(MSG_HEAD_T) - 1);
	memcpy(rsp_ptr, msg_head_ptr, sizeof(MSG_HEAD_T));
	aprsp->status = 0x01;
	ENG_LOG("control charge_flag->cmd=%d\n", charge->cmd);
	if (0x01 == charge->cmd) {
		ret = connect_vbus_charger();
		if (ret > 0) {
			aprsp->status = 0x00;
		} else {
			aprsp->status = 0x01;
		}
	} else if (0x03 == charge->cmd) {
		ret = disconnect_vbus_charger();
		if (ret > 0) {
			aprsp->status = 0x00;
		} else {
			aprsp->status = 0x01;
		}
	} else if (0x02 == charge->cmd) {
		aprsp->status = 0x00;
	}
	msg_head_ptr->len = 8;
	memcpy(rsp, buf, sizeof(MSG_HEAD_T));
	rsp[length - 1] = aprsp->status;
	rsp[length] = 0x7E;
	free(rsp_ptr);

	return length + 1 ;
}

static int calibration_diag_control_charge(char *buf, int len, char *rsp, int rsplen)
{
	int ret = 0, length;
	ENGPC_CHARGER_SWITCH_STATUS_RSP *rtstatus;
	TOOLS_DIAG_AP_CHARGE_CMD *charge;

	if (NULL == buf || NULL == rsp) {
		ENG_LOG("%s,null pointer", __FUNCTION__);
		return 0;
	}

	charge = (TOOLS_DIAG_AP_CHARGE_CMD *)(buf + 1 + sizeof(MSG_HEAD_T) +
					      sizeof(ENGPC_CHARGER_SWITCH_STATUS_RSP));

	rtstatus = (ENGPC_CHARGER_SWITCH_STATUS_RSP *)(rsp + 1 + sizeof(MSG_HEAD_T));

	rtstatus->status = 0x01;
	ENG_LOG("control charge_flag->cmd=%d\n", charge->cmd);
	if (0x01 == charge->cmd) {
		ret = connect_vbus_charger();
		if (ret > 0) {
			rtstatus->status = 0x00;
		} else {
			rtstatus->status = 0x01;
		}
	} else {
		ret = disconnect_vbus_charger();
		if (ret > 0) {
			rtstatus->status = 0x00;
		} else {
			rtstatus->status = 0x01;
		}
	}
	rtstatus->length = 0;

	memcpy(rsp, buf, sizeof(MSG_HEAD_T) + 1);
	length = sizeof(MSG_HEAD_T) + sizeof(ENGPC_CHARGER_SWITCH_STATUS_RSP);
	rsp[5] = length;
	rsp[length + 1] = 0x7E;

	return length + 2;
}

static int eng_diag_read_aux_voltage(char *buf, int len, char *rsp, int rsplen)
{
	int length;
	char *rsp_ptr;
	int ret = 0;

	if (NULL == buf) {
		ENG_LOG("%s,null pointer", __FUNCTION__);
		return 0;
	}
	MSG_HEAD_T *msg_head_ptr = (MSG_HEAD_T *)(buf + 1);
	length = sizeof(TOOLS_DIAG_AP_AUX_VOLTAGE) + sizeof(MSG_HEAD_T);
	rsp_ptr = (char *)malloc(length);
	if (NULL == rsp_ptr) {
		ENG_LOG("%s: Buffer malloc failed\n", __FUNCTION__);
		return 0;
	}
	memcpy(rsp_ptr, msg_head_ptr, sizeof(MSG_HEAD_T));
	TOOLS_DIAG_AP_AUX_VOLTAGE *charge =
			(TOOLS_DIAG_AP_AUX_VOLTAGE *)(buf + 1 + sizeof(MSG_HEAD_T));
	charge->value = ap_get_aux_voltage();
	ENG_LOG("aux charge->value=%d\n", charge->value);
	/* res_cmd=0x100 indicates the voltage display divided by 1000, synchronize with the tool side.*/
	charge->res_cmd = 0x100;
	msg_head_ptr->len = sizeof(MSG_HEAD_T) + sizeof(TOOLS_DIAG_AP_AUX_VOLTAGE);
	memcpy(rsp, buf, 1 + sizeof(MSG_HEAD_T));
	memcpy(rsp + 1 + sizeof(MSG_HEAD_T), charge, sizeof(TOOLS_DIAG_AP_AUX_VOLTAGE));
	rsp[msg_head_ptr->len + 2 - 1] = 0x7E;
	free(rsp_ptr);

	return msg_head_ptr->len + 2 ;
}

static int eng_diag_read_fgu_voltage(char *buf, int len, char *rsp, int rsplen)
{
	if (NULL == buf) {
		ENG_LOG("%s,null pointer", __FUNCTION__);
		return 0;
	}

	memcpy(rsp, buf, 1 + len);
	TOOLS_DIAG_AP_FGU_RESERVED *charge =
			(TOOLS_DIAG_AP_FGU_RESERVED *)(buf + 1 + sizeof(MSG_HEAD_T));
	TOOLS_DIAG_AP_FGU_VOLTAGE *aprsp =
			(TOOLS_DIAG_AP_FGU_VOLTAGE *)(rsp + 1 + sizeof(MSG_HEAD_T) + sizeof(TOOLS_DIAG_AP_FGU_RESERVED));
	if (0x06 == charge->cmd) {
		aprsp->value = ap_get_fgu_voltage();
	} else if (0x07 == charge->cmd) {
		aprsp->value = ap_get_charge_current();
	}

	rsp[9] = 0x0;

	return len ;
}

/*
 * send: 0x7e 0x04 00 00 00 0c 00 62 00 11 00 00 00 7e
 * ack:  0x7e 0x04 00 00 00 0c 00 62 00 00 00 08 00 00 00 00 00 5d ff ff ff 7e
 */
static int eng_diag_read_charge_current(char *buf, int len, char *rsp, int rsplen)
{
	if (NULL == buf) {
		ENG_LOG("%s,null pointer", __FUNCTION__);
		return 0;
	}

	MSG_HEAD_T *msg_head_ptr = (MSG_HEAD_T *)(buf + 1);
	memcpy(rsp, buf, 1 + len);

	TOOLS_DIAG_AP_CHG_CURRENT *charge =
		(TOOLS_DIAG_AP_CHG_CURRENT *)(rsp + 1 + sizeof(MSG_HEAD_T));
	charge->status = 0x00;
	/* the length is charge->reserved + charge->current.*/
	charge->length = 0x08;
	charge->reserved = 0x00;
	charge->current = ap_get_charge_current();
	msg_head_ptr->len = len + sizeof(unsigned int) + sizeof(unsigned int) - 2;
	rsp[5] = msg_head_ptr->len;
	rsp[msg_head_ptr->len + 2 - 1] = 0x7E;

	return msg_head_ptr->len + 2;
}

static int ap_set_charge_current(int cur)
{
	int fd, ret;
	char current[5];

	sprintf(current, "%d", cur);

	fd = open(CHARGER_INPUT_LIMIT_CURRENT_K44, O_WRONLY);
	if (fd >= 0) {
		ret = write(fd, current, 5);
		if (ret < 0) {
			ENG_LOG("set k44 input limit current failed\n");
			close(fd);
			return -1;
		}
		close(fd);
	} else {
		fd = open(CHARGER_INPUT_LIMIT_CURRENT, O_WRONLY);
		if (fd >= 0) {
			ret = write(fd, current, 5);
			if (ret < 0) {
				ENG_LOG("set k414 input limit current failed\n");
				close(fd);
				return -1;
			}
			close(fd);
		}
	}

	fd = open(CHARGER_CURRENT_K44, O_WRONLY);
	if (fd >= 0) {
		ret = write(fd, current, 5);
		if (ret < 0) {
			ENG_LOG("set k44 current failed\n");
			close(fd);
			return -1;
		}
		close(fd);
	} else {
		fd = open(CHARGER_CURRENT, O_WRONLY);
		if (fd >= 0) {
			ret = write(fd, current, 5);
			if (ret < 0) {
				ENG_LOG("set k414 current failed\n");
				close(fd);
				return -1;
			}
			close(fd);
		}
	}

	return 0;
}

/*
 * send: 7E 05 00 00 00 10 00 62 00 /22 00 04 00 F4 01 00 00 7E
 * ack:  7E 05 00 00 00 0C 00 62 00/ 00 00 02 00 7E
 */
static int eng_diag_set_charge_current(char *buf, int len, char *rsp, int rsplen)
{
	if (NULL == buf) {
		ENG_LOG("%s,null pointer", __FUNCTION__);
		return 0;
	}

	memcpy(rsp, buf, 1 + sizeof(MSG_HEAD_T));

	ENGPC_CHARGER_CURRENT_CMD *charger_cmd =
		(ENGPC_CHARGER_CURRENT_CMD *)(buf + 1 + sizeof(MSG_HEAD_T));
	ENGPC_CHARGER_CURRENT_RSP *charger_rsp =
		(ENGPC_CHARGER_CURRENT_RSP *)(rsp + 1 + sizeof(MSG_HEAD_T));

	charger_rsp->status = ap_set_charge_current(charger_cmd->current);
	charger_rsp->length = sizeof(charger_rsp->status);
	rsp[5] = sizeof(MSG_HEAD_T) + sizeof(charger_rsp->status) + sizeof(charger_rsp->length);
	rsplen = rsp[5];
	rsp[rsplen + 2 - 1] = 0x7E;

	return rsplen + 2;
}

static int eng_diag_read_ntc_temperature(char *buf, int len, char *rsp, int rsplen)
{
	char *rsp_ptr;
	TOOLS_DIAG_AP_NTC_TEMPERATURE * bat_ntc = NULL;
	int ret = ENG_DIAG_RET_UNSUPPORT;
	int length;

	if (NULL == buf) {
		ENG_LOG("%s,null pointer", __FUNCTION__);
		return ret;
	}

	ENG_LOG("buf[9]=0x%x\n", buf[9]);
	/* 0x0A is ntc test, Other functions will return ENG_DIAG_RET_UNSUPPORT */
	if(buf[9] != ENG_DIAG_NTC_CMD)
		return ret;

	MSG_HEAD_T *msg_head_ptr = (MSG_HEAD_T *)(buf + 1);
	length =  sizeof(TOOLS_DIAG_AP_NTC_TEMPERATURE) + sizeof(MSG_HEAD_T);
	rsp_ptr = (char *)malloc(length);
	if (NULL == rsp_ptr) {
		ENG_LOG("%s: Buffer malloc failed\n", __FUNCTION__);
		return ret;
	}

	memset(rsp_ptr, '\0', length);
	bat_ntc =  (TOOLS_DIAG_AP_NTC_TEMPERATURE *)(rsp_ptr + sizeof(MSG_HEAD_T));
	msg_head_ptr->len = length;
	bat_ntc->temperature = ap_read_ntc_temperature();
	ENG_LOG("bat_ntc->temperature=%d\n", bat_ntc->temperature);
	memcpy(rsp, buf, 1 + sizeof(MSG_HEAD_T));

	/* Remove the 0x7e at the beginning and end of the buf */
	len = len - 2;

	/* Because the npi tool side is use the formula:
	 * int fTemp=(szRecv[0]+szRecv[1]+szRecv[2]+szRecv[3]-250) / 10
	 * 7E 00 00 00 00 0C 00 38 00 xx xx xx xx 7E
	 */
	bat_ntc->temperature = bat_ntc->temperature + 250;
	for (int i = 0; i < 4; i++)
	{
		if (bat_ntc->temperature < 0) {
			rsp[len + i] = 0x00;
			continue;
		}

		if(bat_ntc->temperature > 255) {
			rsp[len + i] = 0xff;
			bat_ntc->temperature = bat_ntc->temperature - 255;
		} else {
			rsp[len + i] = bat_ntc->temperature;
			break;
		}
	}

	rsp [sizeof(MSG_HEAD_T)] = 0x00;
	rsp[msg_head_ptr->len + 2 - 1] = 0x7E;
	free(rsp_ptr);

	return msg_head_ptr->len + 2;
}

static int eng_diag_get_battery_capacity(char *buf, int len, char *rsp, int rsplen)
{
	TOOLS_DIAG_AP_STATUS *rsp_status = NULL;
	TOOLS_DIAG_AP_CAPACITY *bat_info = NULL;
	MSG_HEAD_T *msg_head_ptr;
	unsigned short read_len;
	char buffer[32] = {0};
	char *rsp_ptr;
	int cap;
	int fd = -1;

	if (NULL == buf) {
		ENG_LOG("%s,null pointer", __FUNCTION__);
		return 0;
	}
	msg_head_ptr = (MSG_HEAD_T *)(rsp + 1);
	rsplen = sizeof(MSG_HEAD_T) + sizeof(TOOLS_DIAG_AP_STATUS) + sizeof(TOOLS_DIAG_AP_CAPACITY);
	rsp_ptr = (char *)malloc(rsplen);
	if (NULL == rsp_ptr) {
		ENG_LOG("%s: Buffer malloc failed\n", __FUNCTION__);
		return 0;
	}

	memset(rsp_ptr, '\0', rsplen);
	rsp_status = (TOOLS_DIAG_AP_STATUS *)(rsp_ptr + sizeof(MSG_HEAD_T));
	bat_info = (TOOLS_DIAG_AP_CAPACITY *)(rsp_ptr + sizeof(MSG_HEAD_T) + sizeof(TOOLS_DIAG_AP_STATUS));

	fd = open(BATTERY_CAPACITY_PATH, O_RDONLY);
	if (fd >= 0) {
		read_len = read(fd, buffer, sizeof(buffer));
		if (read_len > 0) {
			bat_info->capacity = atoi(buffer);
			rsp_status->status = 0x00;
			rsp_status->length = sizeof(TOOLS_DIAG_AP_CAPACITY);
		}
		close(fd);
		ENG_LOG("%s %s capacity = %d, read_len = %d\n", __FUNCTION__,
				BATTERY_CAPACITY_PATH, bat_info->capacity, read_len);
	} else {
		rsp_status->status = 0x01;
		rsp_status->length = 0x00;
		ENG_LOG("%s open %s error!!!\n", __FUNCTION__, BATTERY_CAPACITY_PATH);
	}

	buf[5] = len - 2 + sizeof(TOOLS_DIAG_AP_CAPACITY);
	msg_head_ptr->len = buf[5];
	memcpy(rsp, buf, 1 + sizeof(MSG_HEAD_T));
	memcpy(rsp + 1 + sizeof(MSG_HEAD_T), rsp_status, sizeof(TOOLS_DIAG_AP_STATUS));
	memcpy(rsp + 1 + sizeof(MSG_HEAD_T) + sizeof(TOOLS_DIAG_AP_STATUS), &bat_info->capacity, sizeof(TOOLS_DIAG_AP_CAPACITY));
	rsp[msg_head_ptr->len + 2 - 1] = 0x7E;
	free(rsp_ptr);

	return msg_head_ptr->len + 2;
}

void register_this_module_ext(struct eng_callback *reg, int *num)
{
	int moudles_num = 0;

	ENG_LOG("register_this_module_ext: charger\n");
	reg->type = 0x62;
	reg->subtype = 0x0;
	reg->diag_ap_cmd = 0x30;
	reg->eng_diag_func = eng_diag_get_battery_capacity;
	moudles_num++;

	(reg + moudles_num)->type = 0x38;
	(reg + moudles_num)->subtype = 0x12;
	(reg + moudles_num)->eng_diag_func = eng_diag_control_charge;
	moudles_num++;

	(reg + moudles_num)->type = 0x1e;
	(reg + moudles_num)->subtype = 0x00;
	(reg + moudles_num)->eng_diag_func = eng_diag_read_aux_voltage;
	moudles_num++;

	(reg + moudles_num)->type = 0x62;
	(reg + moudles_num)->subtype = 0x00;
	(reg + moudles_num)->diag_ap_cmd = 0x01;
	(reg + moudles_num)->eng_diag_func = eng_diag_read_fgu_voltage;
	moudles_num++;

	(reg + moudles_num)->type = 0x38;
	(reg + moudles_num)->subtype = 0x0c;
	(reg + moudles_num)->diag_ap_cmd = 0x0a;
	(reg + moudles_num)->eng_diag_func = eng_diag_read_ntc_temperature;
	moudles_num++;

	(reg + moudles_num)->type = 0x62;
	(reg + moudles_num)->subtype = 0x00;
	(reg + moudles_num)->diag_ap_cmd = 0x11;
	(reg + moudles_num)->eng_diag_func = eng_diag_read_charge_current;
	moudles_num++;

	(reg + moudles_num)->type = 0x62;
	(reg + moudles_num)->subtype = 0x00;
	(reg + moudles_num)->diag_ap_cmd = 0x22;
	(reg + moudles_num)->eng_diag_func = eng_diag_set_charge_current;
	moudles_num++;

	(reg + moudles_num)->type = 0x62;
	(reg + moudles_num)->subtype = 0x00;
	(reg + moudles_num)->diag_ap_cmd = 0x10;
	(reg + moudles_num)->eng_diag_func = calibration_diag_control_charge;
	moudles_num++;

	*num = moudles_num;
	ENG_LOG("register_this_module_ext: %d %d", *num, moudles_num);
}
