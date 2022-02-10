#include <stdio.h>
#include <sched.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>

#include <linux/kernel.h>
#include <linux/magic.h>

#include <sys/stat.h>
#include <sys/mount.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <sys/vfs.h>
#include <sys/mman.h>

#include "sprd_fts_type.h"
#include "sprd_fts_log.h"
#include "sprd_fts_diag.h"


/* #define DEBUG */
#ifdef DEBUG
#define DEBUGSEE fprintf
#else
#define DEBUGSEE(...)
#endif

typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;

#define ENG_RAWDATA_FILE "/mnt/vendor/productinfo/dcx_data.dat"
#define PMIC_GLB_VALUE "/sys/bus/platform/drivers/sprd-pmic-glb/sc27xx-syscon/pmic_value"
#define PMIC_GLB_REG "/sys/bus/platform/drivers/sprd-pmic-glb/sc27xx-syscon/pmic_reg"

#define SC2730_CDAC_REG	0X1b14
#define SC2730_HP_AMP_26M	0x1b00
#define SC2730_LP_AMP_26M	0x1b04
#define SC2730_LP_Drift_32K	0x1b20

typedef struct
{
    uint16 CDAC;
    uint16 HP_Mode_Amp_26M;
    uint16 LP_Mode_Amp_26M;
    uint16 LP_Mode_Freq_Drift_32K;
    uint16 reserved[4];
}DIAG_AP_PMIC_AFC_CALI_DATA_T;

static u32 ana_read(u32 reg_addr)
{
	int fd_reg, fd_val, ret;
	char cmds[30] = {0}, ret_val[30] = {0};

	fd_val = open(PMIC_GLB_VALUE, O_RDWR);
	fd_reg = open(PMIC_GLB_REG, O_RDWR);

	sprintf(cmds,"%x",reg_addr);

	ret = write( fd_reg, cmds, sizeof(cmds));
	ret = read( fd_val, ret_val,sizeof(ret_val));
	DEBUGSEE("%s: %d ctrl5:0x cmds:%s, ret_val:%s",__FUNCTION__,__LINE__,cmds,ret_val);

	close(fd_reg);
	close(fd_val);

	return (u32)strtol(ret_val,NULL,16);
}

static u32 ana_write(u32 reg_addr,u32 value)
{
	int fd_reg, fd_val;
	char cmds_addr[30] = {0}, cmds_value[30] = {0};

	fd_val = open(PMIC_GLB_VALUE, O_RDWR);
	fd_reg = open(PMIC_GLB_REG, O_RDWR);

	sprintf(cmds_addr,"%8x", reg_addr);
	write( fd_reg, cmds_addr, sizeof(cmds_addr));
	sprintf(cmds_value,"%8x", value);
	write( fd_val, cmds_value, sizeof(cmds_value));

	close(fd_reg);
	close(fd_val);

	return 0;
}

int read_dcx_data(char *buff, int nLen)
{
	int rcount;
	int ret = 0;
	FILE * fp = NULL;

	if(NULL == buff)
	{
		ENG_LOG("%s: res is NULL!!!",__FUNCTION__);
		ret = -1;
		return ret;
	}

	if(access(ENG_RAWDATA_FILE, F_OK) == 0) {
		ENG_LOG("%s: %s exists",__FUNCTION__, ENG_RAWDATA_FILE);
		fp = fopen(ENG_RAWDATA_FILE, "r");
		if(NULL == fp)
		{
		  ENG_LOG("%s: fopen fail errno=%d, strerror(errno)=%s",__FUNCTION__, errno, strerror(errno));
		  ret = -1;
		  return ret;
		}

		rcount = fread(buff, sizeof(char), nLen, fp);

		if(rcount <= 0)
		{
		  ret = -1;
		}

		ENG_LOG("%s: fread count %d",__FUNCTION__, rcount);
		fclose(fp);
	} else {
		ret = -1;
		ENG_LOG("%s: %s not exists",__FUNCTION__, ENG_RAWDATA_FILE);
		return ret;
	}
	return rcount;
}

int main() {
	int ret = 0;
	DIAG_AP_PMIC_AFC_CALI_DATA_T pmicCaliData;

	ENG_LOG("dcxsrv start............");
	ret = read_dcx_data(&pmicCaliData, sizeof(DIAG_AP_PMIC_AFC_CALI_DATA_T));
	if (ret < 0) {
		ENG_LOG("%s: fread count %d",__FUNCTION__, ret);
		return 0;
	}

	ana_write(SC2730_CDAC_REG, pmicCaliData.CDAC);
	ana_write(SC2730_HP_AMP_26M, pmicCaliData.HP_Mode_Amp_26M);
	ana_write(SC2730_LP_AMP_26M, pmicCaliData.LP_Mode_Amp_26M);
	ana_write(SC2730_LP_Drift_32K, pmicCaliData.LP_Mode_Freq_Drift_32K);

	ENG_LOG("dcxsrv end........");

  return 0;
}
