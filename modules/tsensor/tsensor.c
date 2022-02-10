#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
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
#include "sprd_fts_diag.h"
#include "sprd_fts_log.h"

#define BIT(x) (1<<x)
#define	SC27XX_TSEN_CTRL0	0x1b34
#define SC27XX_TSEN_CLK_SRC_SEL	BIT(4)
#define	SC27XX_TSEN_ADCLDO_EN	BIT(15)
#define	SC27XX_TSEN_CTRL1	0x1b38
#define SC27XX_TSEN_SDADC_EN	BIT(11)
#define	SC27XX_TSEN_UGBUF_EN	BIT(14)
#define	SC27XX_TSEN_CTRL3	0x1b40
#define	SC27XX_TSEN_EN		BIT(0)
#define	SC27XX_TSEN_SEL_EN	BIT(3)
#define	SC27XX_TSEN_SEL_CH	BIT(4)
#define	SC27XX_TSEN_CTRL4	0x1b44
#define	SC27XX_TSEN_CTRL5	0x1b48


#define PMIC_GLB_VALUE "/sys/bus/platform/drivers/sprd-pmic-glb/sc27xx-syscon/pmic_value"
#define PMIC_GLB_REG "/sys/bus/platform/drivers/sprd-pmic-glb/sc27xx-syscon/pmic_reg"
#define TSENSOR_DCXO_TEMP "/sys/class/thermal/thermal_zone10/temp"
#define TSENSOR_CRYSTAL_TEMP "/sys/class/thermal/thermal_zone11/temp"

const char* CONFIG_FILE_SENSOR_PATH = "/sys/class/thermal";
static char osc_path[64];
static char outsen_path[64];
static int result = 0;
static bool inited = 0;

typedef struct
{
    unsigned int tsx_temp;    // TSX TEMP*100
    unsigned int osc_temp;    //OSC TEMP*100
    unsigned short reserved[2];
}DIAG_AP_PMIC_AFC_TSX_TEMP_T;

/*typedef struct {
       unsigned short status;
       unsigned short length;
} TOOLS_DIAG_AP_CNF_T;*/

static unsigned int ana_read(unsigned int reg_addr)
{
	int fd_reg, fd_val, ret;
	char cmds[30] = {0}, ret_val[30] = {0};

	fd_val = open(PMIC_GLB_VALUE, O_RDWR);
	fd_reg = open(PMIC_GLB_REG, O_RDWR);
	sprintf(cmds,"%x",reg_addr);
	ret = write( fd_reg, cmds, sizeof(cmds));
	ret = read( fd_val, ret_val,sizeof(ret_val));
	ENG_LOG("%s: cmds:%s, ret_val:%s",__FUNCTION__,cmds,ret_val);
	close(fd_reg);
	close(fd_val);
	return (unsigned int)strtol(ret_val,NULL,16);
}

static unsigned int ana_write(unsigned int reg_addr,unsigned int value)
{
	int fd_reg, fd_val;
	char cmds_addr[30] = {0}, cmds_value[30] = {0};
	fd_val = open(PMIC_GLB_VALUE, O_RDWR);
	fd_reg = open(PMIC_GLB_REG, O_RDWR);
	sprintf(cmds_addr,"%8x", reg_addr);
	write( fd_reg, cmds_addr, sizeof(cmds_addr));
	sprintf(cmds_value,"%8x", value);
	write( fd_val, cmds_value, sizeof(cmds_value));
	ENG_LOG("%s: reg_addr:0x%x val:0x%x",__FUNCTION__,reg_addr,value);
	close(fd_reg);
	close(fd_val);
	usleep(10000);
	return 0;
}

static void tsensor_regmap_update_bits(unsigned int reg, unsigned int mask, unsigned int val)
{
	unsigned int tmp, orig;

	orig = ana_read(reg);
	tmp = orig & ~mask;
	tmp |= val & mask;
	ana_write(reg, tmp);
}

static void tsenor_restore_default_reg(void)
{
	tsensor_regmap_update_bits(SC27XX_TSEN_CTRL0, SC27XX_TSEN_CLK_SRC_SEL, SC27XX_TSEN_CLK_SRC_SEL);
	tsensor_regmap_update_bits(SC27XX_TSEN_CTRL3, SC27XX_TSEN_EN | SC27XX_TSEN_SEL_EN | SC27XX_TSEN_SEL_CH, 0);
	tsensor_regmap_update_bits(SC27XX_TSEN_CTRL1, SC27XX_TSEN_SDADC_EN | SC27XX_TSEN_UGBUF_EN, 0);
	tsensor_regmap_update_bits(SC27XX_TSEN_CTRL0, SC27XX_TSEN_ADCLDO_EN, 0);
	inited = 0;
}

static int tsensor_match_thm_zone(const char* type)
{
  int i = 0;
  int fd, len;
  char path[64];
  char buf[16];

  while (1){
    sprintf(path, "%s/thermal_zone%d/type", CONFIG_FILE_SENSOR_PATH, i);
    fd = open(path, O_RDONLY);
    if (fd < 0){
      return -1;
    }
    len = read(fd, buf, 15);
    close(fd);
    if (len < 0){
      return -1;
    }
    buf[len] = '\0';
    ENG_LOG("get thermal_zone%d type: %s", i, buf);
    if (!strncmp(buf, type, strlen(type))){
	    return i;
	}
    ++i;
  }
  return -1;
}

static void tsensor_get_thmzone_patch(void)
{
	int i;

	i= tsensor_match_thm_zone("osctsen-thmzone");
	if(i >= 0)
		sprintf(osc_path, "%s/thermal_zone%d/temp", CONFIG_FILE_SENSOR_PATH, i);
	else
		result = -1;
	i= tsensor_match_thm_zone("outtsen-thmzone");
	if(i >= 0)
		sprintf(outsen_path, "%s/thermal_zone%d/temp", CONFIG_FILE_SENSOR_PATH, i);
	else
		result = -1;
}

static int ap_read_tsx_temperature(unsigned int *temp)
{
	int fd = -1;
	int read_len = 0;
	char buffer[64] = {0};

	fd = open(outsen_path, O_RDONLY);

	if (fd >= 0) {
		read_len = read(fd, buffer, sizeof(buffer));
		if (read_len > 0)
			*temp = atoi(buffer);

		close(fd);
	}

	return fd;
}

static int ap_read_osc_temperature(unsigned int *temp)
{
	int fd = -1;
	int read_len = 0;
	char buffer[64] = {0};

	fd = open(osc_path, O_RDONLY);

	if (fd >= 0) {
		read_len = read(fd, buffer, sizeof(buffer));
		if (read_len > 0)
			*temp = atoi(buffer);

		close(fd);
	}

	return fd;
}

static int eng_diag_read_tsx_osc_temp(char *buf, int len, char *rsp, int rsplen)
{
	TOOLS_DIAG_AP_CNF_T *rsp_status = NULL;
	DIAG_AP_PMIC_AFC_TSX_TEMP_T *tsenor = NULL;
	MSG_HEAD_T *msg_head_ptr;
	char *rsp_ptr;
	unsigned int length;

	if (NULL == buf) {
		ENG_LOG("%s,null pointer", __FUNCTION__);
		return 0;
	}
	msg_head_ptr = (MSG_HEAD_T *)(buf + 1);
	length = sizeof(MSG_HEAD_T) + sizeof(TOOLS_DIAG_AP_CNF_T) + sizeof(DIAG_AP_PMIC_AFC_TSX_TEMP_T);
	rsp_ptr = (char *)malloc(length);
	if (NULL == rsp_ptr) {
		ENG_LOG("%s: Buffer malloc failed\n", __FUNCTION__);
		return 0;
	}

	memset(rsp_ptr, '\0', length);
	rsp_status = (TOOLS_DIAG_AP_CNF_T *)(rsp_ptr + sizeof(MSG_HEAD_T));
	rsp_status->status = 0x00; //error fail
	tsenor = (DIAG_AP_PMIC_AFC_TSX_TEMP_T *)(rsp_ptr + sizeof(MSG_HEAD_T) + sizeof(TOOLS_DIAG_AP_CNF_T));

	if(result < 0){
			rsp_status->status = 0x01; //error fail
	}

	if(!inited) {
		if(ap_read_tsx_temperature(&tsenor->tsx_temp) < 0){
			ENG_LOG("%s: read tsx failed\n", __FUNCTION__);
		}
		if(ap_read_osc_temperature(&tsenor->osc_temp) < 0){
			ENG_LOG("%s: read osc failed\n", __FUNCTION__);
		}
		usleep(480 * 1000);
		inited = 1;
	}

	if(ap_read_tsx_temperature(&tsenor->tsx_temp) < 0){
			rsp_status->status = 0x01; //error fail
	}
	if(ap_read_osc_temperature(&tsenor->osc_temp) < 0){
			rsp_status->status = 0x01; //error fail
	}

	tsenor->reserved[0]=0x0;
    	tsenor->reserved[1]=0x0;
	rsp_status->length = sizeof(DIAG_AP_PMIC_AFC_TSX_TEMP_T);

	msg_head_ptr->len = length;
	memcpy(rsp, buf, 1 + sizeof(MSG_HEAD_T));
	memcpy(rsp + 1 + sizeof(MSG_HEAD_T), rsp_status, sizeof(TOOLS_DIAG_AP_CNF_T));
	memcpy(rsp + 1 + sizeof(MSG_HEAD_T) + sizeof(TOOLS_DIAG_AP_CNF_T), tsenor, sizeof(DIAG_AP_PMIC_AFC_TSX_TEMP_T));
	rsp[length + 2 - 1] = 0x7E;
	free(rsp_ptr);

	ENG_LOG("%s: tsensor tsx_temp %d, osc temp %d\n", __FUNCTION__, tsenor->tsx_temp, tsenor->osc_temp);
	return length + 2;
}

static int eng_diag_restore_default_reg(char *buf, int len, char *rsp, int rsplen)
{
	TOOLS_DIAG_AP_CNF_T *rsp_status = NULL;
	MSG_HEAD_T *msg_head_ptr;
	char *rsp_ptr;
	unsigned int length;

	if (NULL == buf) {
		ENG_LOG("%s,null pointer", __FUNCTION__);
		return 0;
	}
	msg_head_ptr = (MSG_HEAD_T *)(buf + 1);
	length = sizeof(MSG_HEAD_T) + sizeof(TOOLS_DIAG_AP_CNF_T);
	rsp_ptr = (char *)malloc(length);
	if (NULL == rsp_ptr) {
		ENG_LOG("%s: Buffer malloc failed\n", __FUNCTION__);
		return 0;
	}

	tsenor_restore_default_reg();
	memset(rsp_ptr, '\0', length);
	rsp_status = (TOOLS_DIAG_AP_CNF_T *)(rsp_ptr + sizeof(MSG_HEAD_T));
	rsp_status->status = 0x00; //error fail
	rsp_status->length = 0x00;

	msg_head_ptr->len = length;
	memcpy(rsp, buf, 1 + sizeof(MSG_HEAD_T));
	memcpy(rsp + 1 + sizeof(MSG_HEAD_T), rsp_status, sizeof(TOOLS_DIAG_AP_CNF_T));
	rsp[length + 2 - 1] = 0x7E;
	free(rsp_ptr);

	ENG_LOG("%s: tsensor registor restore default val \n", __FUNCTION__);
	return length + 2;
}

void register_this_module_ext(struct eng_callback *reg, int *num)
{
	int moudles_num = 0;

	ENG_LOG("register_this_module_ext: tsensor\n");
	tsensor_get_thmzone_patch();
	ENG_LOG("osc %s, out %s \n", osc_path, outsen_path);

	(reg + moudles_num)->type = 0x62;
	(reg + moudles_num)->subtype = 0x0;
	(reg + moudles_num)->diag_ap_cmd = 0x31;
	(reg + moudles_num)->eng_diag_func = eng_diag_read_tsx_osc_temp;
	moudles_num++;

	(reg + moudles_num)->type = 0x62;
	(reg + moudles_num)->subtype = 0x0;
	(reg + moudles_num)->diag_ap_cmd = 0x32;
	(reg + moudles_num)->eng_diag_func = eng_diag_restore_default_reg;
	moudles_num++;

	*num = moudles_num;
	ENG_LOG("register_this_module_ext: %d %d", *num, moudles_num);
}
