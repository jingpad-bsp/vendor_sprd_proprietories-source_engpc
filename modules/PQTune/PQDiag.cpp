#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <math.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <sys/sysinfo.h>
#include <ctype.h>
#include <sys/reboot.h>
#include <utils/Log.h>
#include "PQTuneCore.h"
#include "PQSysfs.h"

static PQTuneCore *TuneCore = NULL;

static int pq_cmd_connect(char *buf, int len, char *rsp, int rsplen)
{
	int rsp_len = 0;

	if (TuneCore)
		rsp_len = TuneCore->tune_connect(buf, len, rsp, rsplen);
	
	return rsp_len;
}

static int pq_cmd_rgb_pattern(char *buf, int len, char *rsp, int rsplen)
{
	int rsp_len = 0;

	if (TuneCore)
		rsp_len = TuneCore->tune_rgb_pattern(buf, len, rsp, rsplen);
	

	return rsp_len;
}

static int pq_cmd_read_regs(char *buf, int len, char *rsp, int rsplen)
{
	int rsp_len = 0;

	if (TuneCore)
		rsp_len = TuneCore->tune_read_regs(buf, len, rsp, rsplen);
		

	return rsp_len;
	
}

static int pq_cmd_write_regs(char *buf, int len, char *rsp, int rsplen)
{
	int rsp_len = 0;

	if (TuneCore)
		rsp_len = TuneCore->tune_write_regs(buf, len, rsp, rsplen);
		
	return rsp_len;
}

static int pq_cmd_start_read_cfg(char *buf, int len, char *rsp, int rsplen)
{
	int rsp_len = 0;

	
	if (TuneCore)
		rsp_len = TuneCore->tune_start_read_cfg(buf, len, rsp, rsplen);
	

	return rsp_len;
}

static int pq_cmd_midst_read_cfg(char *buf, int len, char *rsp, int rsplen)
{
	int rsp_len = 0;

	if (TuneCore)
		rsp_len = TuneCore->tune_midst_read_cfg(buf, len, rsp, rsplen);
		

	return rsp_len;
}

static int pq_cmd_end_read_cfg(char *buf, int len, char *rsp, int rsplen)
{
	int rsp_len = 0;

	if (TuneCore)
		rsp_len = TuneCore->tune_end_read_cfg(buf, len, rsp, rsplen);


	return rsp_len;
}

static int pq_cmd_start_write_cfg(char *buf, int len, char *rsp, int rsplen)
{
	int rsp_len = 0;

	if (TuneCore)
		rsp_len = TuneCore->tune_start_write_cfg(buf, len, rsp, rsplen);

	return rsp_len;
}

static int pq_cmd_midst_write_cfg(char *buf, int len, char *rsp, int rsplen)
{
	int rsp_len = 0;

	if (TuneCore)
		rsp_len = TuneCore->tune_midst_write_cfg(buf, len, rsp, rsplen);

	return rsp_len;
}

static int pq_cmd_end_write_cfg(char *buf, int len, char *rsp, int rsplen)
{
	int rsp_len = 0;

	if (TuneCore)
		rsp_len = TuneCore->tune_end_write_cfg(buf, len, rsp, rsplen);

	return rsp_len;
}

static int pq_cmd_rd_tuning_reg(char *buf, int len, char *rsp, int rsplen)
{
	int rsp_len = 0;

	if (TuneCore)
		rsp_len = TuneCore->tune_rd_tuning_reg(buf, len, rsp, rsplen);

	return rsp_len;
}

static int pq_cmd_wr_tuning_reg(char *buf, int len, char *rsp, int rsplen)
{
	int rsp_len = 0;

	if (TuneCore)
		rsp_len = TuneCore->tune_wr_tuning_reg(buf, len, rsp, rsplen);

	return rsp_len;
}

int pq_cmd_rd_tuning_xml(char *buf, int len, char *rsp, int rsplen)
{
	int rsp_len = 0;

	if (TuneCore)
		rsp_len = TuneCore->tune_rd_tuning_xml(buf, len, rsp, rsplen);
	
	return rsp_len;
}

int pq_cmd_wr_tuning_xml(char *buf, int len, char *rsp, int rsplen)
{
	int rsp_len = 0;
	if (TuneCore)
		rsp_len = TuneCore->tune_wr_tuning_xml(buf, len, rsp, rsplen);

	return rsp_len;
}

int pq_cmd_rd_ambient(char *buf, int len, char *rsp, int rsplen)
{
	int rsp_len = 0;

	if (TuneCore)
		rsp_len = TuneCore->tune_rd_ambient(buf, len, rsp, rsplen);

	return rsp_len;
}

PQTuneCore* pq_create_TuneCore(char *version)
{
	if (!strncmp(version, "dpu-lite-r2p0", strlen("dpu-lite-r2p0"))){
		return new PQTuneCoreLiteR2p0(DPU_LITE_R2P0);
	} else if (!strncmp(version, "dpu-r2p0", strlen("dpu-r2p0"))) {
		return 	new PQTuneCore(DPU_R2P0);
	} else if (!strncmp(version, "dpu-r3p0", strlen("dpu-r3p0")))
		return new PQTuneCoreR3p0(DPU_R3P0);
	else if (!strncmp(version, "dpu-r4p0", strlen("dpu-r4p0")))
		return new PQTuneCoreR4p0(DPU_R4P0);
	else 
		return NULL;
}

int pq_adapt_dpu_core(void)
{
	int fd;
	int ret = 0;
	char version[20] = {0};

	fd = open(DpuVersion, O_RDONLY);
	if (fd < 0) {
		ALOGD("%s: open file failed, err: %s\n", __func__,
			strerror(errno));
		return errno;
	}

	ret = read(fd, version, sizeof(version));
	TuneCore = pq_create_TuneCore(version);
	close(fd);

	return 0;
}

extern "C" void register_this_module_ext(struct eng_callback *reg, int *num) {
  int moudles_num = 0;
  int ret = 0;
  struct eng_callback *tmp = reg;
  ALOGD("register_this_module_ext libPQTune");

  ret = pq_adapt_dpu_core();
  if (ret) {
  	ALOGD("PQ adapt dpu ip fail");
	//TuneCore = dpu_r2p0_list.ops;
	return;
  }
  /***PQ Connect Cmd***/
  tmp->type = 0x5D;   // main cmd
  tmp->subtype = 0x01; // sub cmd
  tmp->diag_ap_cmd = 0x1;
  tmp->eng_diag_func = pq_cmd_connect; // rsp function ptr
  moudles_num++;
  tmp++;

  /***PQ Gamma Cmd***/
  tmp->type = 0x5D;   // main cmd
  tmp->subtype = 0x1; // sub cmd
  tmp->diag_ap_cmd = 0x2;
  tmp->eng_diag_func = pq_cmd_rgb_pattern; // rsp function ptr
  moudles_num++;
  tmp++;

   /***PQ Read Regs Cmd***/
  tmp->type = 0x5D;   // main cmd
  tmp->subtype = 0x1; // sub cmd
  tmp->diag_ap_cmd = 0x3;
  tmp->eng_diag_func = pq_cmd_read_regs; // rsp function ptr
  moudles_num++;
  tmp++;

   /***PQ Write Regs Cmd***/
  tmp->type = 0x5D;   // main cmd
  tmp->subtype = 0x1; // sub cmd
  tmp->diag_ap_cmd = 0x4;
  tmp->eng_diag_func = pq_cmd_write_regs; // rsp function ptr
  moudles_num++;
  tmp++;

    /***PQ cfg read Cmd***/
  tmp->type = 0x5D;   // main cmd
  tmp->subtype = 0x1; // sub cmd
  tmp->diag_ap_cmd = 0x5;
  tmp->eng_diag_func = pq_cmd_start_read_cfg; // rsp function ptr
  moudles_num++;
  tmp++;
     /***PQ cfg read Cmd***/
  tmp->type = 0x5D;   // main cmd
  tmp->subtype = 0x1; // sub cmd
  tmp->diag_ap_cmd = 0x6;
  tmp->eng_diag_func = pq_cmd_midst_read_cfg; // rsp function ptr
  moudles_num++;
  tmp++;

    /***PQ cfg read Cmd***/
  tmp->type = 0x5D;   // main cmd
  tmp->subtype = 0x1; // sub cmd
  tmp->diag_ap_cmd = 0x7;
  tmp->eng_diag_func = pq_cmd_end_read_cfg; // rsp function ptr
  moudles_num++;
  tmp++;

    /***PQ cfg write Cmd***/
  tmp->type = 0x5D;   // main cmd
  tmp->subtype = 0x1; // sub cmd
  tmp->diag_ap_cmd = 0x8;
  tmp->eng_diag_func = pq_cmd_start_write_cfg; // rsp function ptr
  moudles_num++;
  tmp++;

    /***PQ cfg write Cmd***/
  tmp->type = 0x5D;   // main cmd
  tmp->subtype = 0x1; // sub cmd
  tmp->diag_ap_cmd = 0x9;
  tmp->eng_diag_func = pq_cmd_midst_write_cfg; // rsp function ptr
  moudles_num++;
  tmp++;

   /***PQ Connect Cmd***/
  tmp->type = 0x5D;   // main cmd
  tmp->subtype = 0x1; // sub cmd
  tmp->diag_ap_cmd = 0xa;
  tmp->eng_diag_func = pq_cmd_end_write_cfg; // rsp function ptr
  moudles_num++;
  tmp++;

    /***PQ Read Tuning regs Cmd***/
  tmp->type = 0x5D;   // main cmd
  tmp->subtype = 0x1; // sub cmd
  tmp->diag_ap_cmd = 0xb;
  tmp->eng_diag_func = pq_cmd_rd_tuning_reg; // rsp function ptr
  moudles_num++;
  tmp++;

    /***PQ Write Tunning regs Cmd***/
  tmp->type = 0x5D;   // main cmd
  tmp->subtype = 0x1; // sub cmd
  tmp->diag_ap_cmd = 0xc;
  tmp->eng_diag_func = pq_cmd_wr_tuning_reg; // rsp function ptr
  moudles_num++;
  tmp++;

    /***PQ Tuning xml Cmd***/
  tmp->type = 0x5D;   // main cmd
  tmp->subtype = 0x1; // sub cmd
  tmp->diag_ap_cmd = 0xd;
  tmp->eng_diag_func = pq_cmd_rd_tuning_xml; // rsp function ptr
  moudles_num++;
  tmp++;

   /***PQ Tuning xml Cmd***/
  tmp->type = 0x5D;   // main cmd
  tmp->subtype = 0x1; // sub cmd
  tmp->diag_ap_cmd = 0xe;
  tmp->eng_diag_func = pq_cmd_wr_tuning_xml; // rsp function ptr
  moudles_num++;
  tmp++;

    /***PQ Read Ambient Cmd***/
  tmp->type = 0x5D;   // main cmd
  tmp->subtype = 0x1; // sub cmd
  tmp->diag_ap_cmd = 0xf;
  tmp->eng_diag_func = pq_cmd_rd_ambient; // rsp function ptr
  moudles_num++;

  *num = moudles_num;

  ALOGD("register_this_module_ext: %d - %d", *num, moudles_num);
}


