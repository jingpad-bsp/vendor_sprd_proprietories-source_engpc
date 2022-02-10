#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/sysinfo.h>
#include <ctype.h>
#include <utils/Log.h>
#include "PQTuningParmDefine.h"
#include "PQTuneCore.h"
#include "PQDiag.h"
#include "PQSysfs.h"
#include "Minui/minui.h"


static void HexStrstoInt(char *buf, int size)
{
	int *temp = (int*) buf;
	uint32_t i = 0;
	uint32_t j = 0;
	for(; i < size; i++){
		temp[i] = strtol((const char *)(buf + j), (char**)0, 16);
		j += 9;
	}   
}

static int eng_diag_encode7d7e(char *buf, int len, int *extra_len) {
	int i, j;
	char tmp;


	for (i = 0; i < len; i++) {
		if ((buf[i] == 0x7d) || (buf[i] == 0x7e)) {
			tmp = buf[i] ^ 0x20;
			buf[i] = 0x7d;
			for (j = len; j > i + 1; j--) {
				buf[j] = buf[j - 1]; 
			}
			buf[i + 1] = tmp;
			len++;
			(*extra_len)++;

			for (j = 0; j < len; j++) {
				printf("%x,", buf[j]);
			}
		}
	}   

	return len;
}

static int MultTimes(int base, int n)
{
	int mt = 1;
	int m;

	for(m = 0; m < n; m++)
		mt *= base;
	return mt; 
}

static void parse_panelsize(char *data, int count, DUT_INFO_T  *dut_info_t)
{
	unsigned char i;
	unsigned char j;
	unsigned char next = 0;
	unsigned char last = 0;

	dut_info_t->stResolution.HPixel = 0;
	dut_info_t->stResolution.VPixel = 0;
	for (i = 0; i< count; i++){
		if (data[i] == 'x'){
			next = i + 1;
			for (j=0; j < i-1; j++){
				dut_info_t->stResolution.HPixel += (data[j]- 48)*(MultTimes(10, i-1-j));
			}   
		}   
		if(data[i] == '\n'){
			last = i -1; 
			for(j = 0; j < (last-next+1); j++){
				dut_info_t->stResolution.VPixel += (data[next+j]- 48)*(MultTimes(10, last-next-j));
			}   

		}
	}   

}

static void tune_modules_enable(int version, void *ctx,int enable)
{
	switch(version){
		case DPU_R2P0:
			((pq_tuning_parm *)ctx)->gamma.version.enable = enable;
			((pq_tuning_parm *)ctx)->abc.version.enable = enable;
			((pq_tuning_parm *)ctx)->cms.version.enable = enable;
			((pq_tuning_parm *)ctx)->bld.version.enable = enable;
			break;
		case DPU_LITE_R2P0:
			((pq_tuning_parm_sharkl5 *)ctx)->gamma.version.enable = enable;
			((pq_tuning_parm_sharkl5 *)ctx)->abc.version.enable = enable;
			((pq_tuning_parm_sharkl5 *)ctx)->cms.version.enable = enable;
			((pq_tuning_parm_sharkl5 *)ctx)->bld.version.enable = enable;
			break;
		case DPU_R3P0:
			((pq_tuning_parm_roc1 *)ctx)->gamma.version.enable = enable;
			((pq_tuning_parm_roc1 *)ctx)->abc.version.enable = enable;
			((pq_tuning_parm_roc1 *)ctx)->cms.version.enable = enable;
			((pq_tuning_parm_roc1 *)ctx)->bld.version.enable = enable;
			break;
		case DPU_R4P0:
			((pq_tuning_parm_sharkl5Pro *)ctx)->gamma.version.enable = enable;
			((pq_tuning_parm_sharkl5Pro *)ctx)->abc.version.enable = enable;
			((pq_tuning_parm_sharkl5Pro *)ctx)->cms.version.enable = enable;
			((pq_tuning_parm_sharkl5Pro *)ctx)->bld.version.enable = enable;
			break;
		default:
			return;
		}
}

static void tune_module_enable(int version, void *ctx, int module)
{
	switch(version){
		case DPU_R2P0:
			if (module & GAMMA_EN)
				((pq_tuning_parm *)ctx)->gamma.version.enable = 1;
			if (module & SLP_EN)
				((pq_tuning_parm *)ctx)->abc.version.enable = 1;
			if (module & CMS_EN)
				((pq_tuning_parm *)ctx)->cms.version.enable = 1;
			if (module & HSV_EN)
				((pq_tuning_parm *)ctx)->bld.version.enable = 1;
			break;
		case DPU_LITE_R2P0:
			if (module & GAMMA_EN)
				((pq_tuning_parm_sharkl5 *)ctx)->gamma.version.enable = 1;
			if (module & SLP_EN)
				((pq_tuning_parm_sharkl5 *)ctx)->abc.version.enable = 1;
			if (module & CMS_EN)
				((pq_tuning_parm_sharkl5 *)ctx)->cms.version.enable = 1;
			if (module & HSV_EN)
				((pq_tuning_parm_sharkl5 *)ctx)->bld.version.enable = 1;
			break;
		case DPU_R3P0:
			if (module & GAMMA_EN)
				((pq_tuning_parm_roc1 *)ctx)->gamma.version.enable = 1;
			if (module & SLP_EN)
				((pq_tuning_parm_roc1 *)ctx)->abc.version.enable = 1;
			if (module & CMS_EN)
				((pq_tuning_parm_roc1 *)ctx)->cms.version.enable = 1;
			if (module & HSV_EN)
				((pq_tuning_parm_roc1 *)ctx)->bld.version.enable = 1;
			break;
		case DPU_R4P0:
			if (module & GAMMA_EN)
				((pq_tuning_parm_sharkl5Pro *)ctx)->gamma.version.enable = 1;
			if (module & SLP_EN)
				((pq_tuning_parm_sharkl5Pro *)ctx)->abc.version.enable = 1;
			if (module & CMS_EN)
				((pq_tuning_parm_sharkl5Pro *)ctx)->cms.version.enable = 1;
			if (module & HSV_EN)
				((pq_tuning_parm_sharkl5Pro *)ctx)->bld.version.enable = 1;
			break;

		default:
			return;
		}
}

static void tune_version_copy(int version, char *dpuname)
{
	switch(version){
		case DPU_R2P0:
			strncpy(dpuname, "dpu-r2p0", strlen("dpu-r2p0"));
			break;
		case DPU_LITE_R2P0:
			strncpy(dpuname, "dpu-lite-r2p0", strlen("dpu-lite-r2p0"));
			break;
		case DPU_R3P0:
			strncpy(dpuname, "dpu-r3p0", strlen("dpu-r3p0"));
			break;
		case DPU_R4P0:
			strncpy(dpuname, "dpu-r4p0", strlen("dpu-r4p0"));
			break;
		default:
			return;
		}
}

PQTuneCore::PQTuneCore(int ver)
{
	if (ver != DPU_R2P0)
		return;

	version = ver;
	abc = new AbcParser();
	if (!abc) {
		ALOGD("%s abc create fail\n", __func__);
		return;
	}
	cms = new CmsParser();
	if (!cms) {
		ALOGD("%s cms create fail\n", __func__);
		return;
	}
	bld = new BldParser();
	if (!bld) {
		ALOGD("%s bld create fail\n", __func__);
		return;
	}
	gamma = new GammaParser();
	if (!gamma) {
		ALOGD("%s gamma create fail\n", __func__);
		return;
	}
	hsv = NULL;
	hsv_size = 0;
	ctx = (uint08_t *)malloc(sizeof(pq_tuning_parm));
	if (!ctx) {
		ALOGD("%s ctx create fail\n", __func__);
		return;
	}
	tune_sizes = sizeof(pq_tuning_parm);
	gamma_size = sizeof(gamma_common);
	bld_size = sizeof(bld_common);
	cms_size = sizeof(cms_common);
	abc_size = sizeof(abc_common);
	offset = 0;

	ALOGD("%S created\n", __func__);
}

PQTuneCore:: ~PQTuneCore()
{
	if (abc)
		delete abc;
	if (cms)
		delete cms;
	if (bld)
		delete bld;
	if (gamma)
		delete gamma;
	if (ctx)
		free(ctx);
}

int  PQTuneCore::tune_version()
{
	if (version)
		return version;
	else
		return -1;
}

int PQTuneCore::tune_connect(char *buf, int len, char *rsp, int rsplen)
{
	DUT_INFO_T  *dut_info;
	MSG_HEAD_T *rsp_head;
	uint32_t rsp_len = 0;
	char info[1024] = {0};
	char backlight[10] = {0};
	int sizes = 0;
	int fd;
	int fd1;
	int fd2;
	int fd3;
	char *pchar;
	char *ptemp;
	unsigned long status = 0;
	int ret;

	fd = open(PanelSize, O_RDONLY);
	fd1 = open(ChipInfo, O_RDONLY);
	fd2 = open(PQStatus, O_RDONLY);
	fd3 = open(Brightness, O_RDWR); 
	if (fd < 0 || fd1 < 0 || fd2 < 0 || fd3 < 0) {
		ALOGD("%s: open file failed, err: %s\n", __func__,
			strerror(errno));
		if (fd >= 0)
			close(fd);
		if (fd1 >= 0)
			close(fd1);
		if (fd2 >= 0)
			close(fd2);
		if (fd3 >= 0)
			close(fd3);
		return errno;
	}

	if (version == DPU_R2P0)
		ALOGD("adf display\n");
	else {
		ret = gr_init();
		if(ret)
			ALOGD("PQ display init fail\n");
	}

	dut_info =  (DUT_INFO_T  *)malloc(sizeof(DUT_INFO_T));
	if(!dut_info) {
		close(fd);
		close(fd1);
		close(fd2);
		close(fd3);
		printf("PQ dut info alloc fail\n");
		return -1;
	}

	snprintf(backlight, sizeof(backlight), "%d", 255);
	write(fd3, backlight, sizeof(backlight));

	ret = read(fd2, info, 100);
	if (ret < 0) {
		printf("PQ display sleep in\n");
		tune_modules_enable(version, ctx, 0);
	} else if (ret > 0) {
		pchar = strstr(info, "0x");
		if (pchar) {
			status = strtol(pchar, NULL, 16);
			tune_module_enable(version, ctx, status);
		}
	}
	ALOGD("PQ status %x\n", status);
	rsp_head = (MSG_HEAD_T *)(rsp + 1);
	memset(dut_info, 0, sizeof(DUT_INFO_T));
	sizes = read(fd1, info, 1024);
	if (sizes < 0) {
		close(fd);
		close(fd1);
		close(fd2);
		close(fd3);
		free(dut_info);
		return -1;
	}
	pchar = strstr(info, "androidboot.hardware");
	if (pchar == NULL) {
		close(fd);
		close(fd1);
		close(fd2);
		close(fd3);
		free(dut_info);
		return -1;
	}
	pchar = strstr(pchar, "=");
	if (pchar == NULL) {
		close(fd);
		close(fd1);
		close(fd2);
		close(fd3);
		free(dut_info);
		return -1;
	}
	ptemp = strchr(pchar + 1, ' ');
	sizes = ptemp - pchar - 1;
	tune_version_copy(version, dut_info->szModelName);
	strncpy(dut_info->szChipName, pchar + 1, sizes);
	sizes = read(fd, info, 1024);
	if (sizes < 0) {
		close(fd);
		close(fd1);
		close(fd2);
		close(fd3);
		free(dut_info);
		return -1;
        }
	parse_panelsize(info, sizes , dut_info);
	memcpy(rsp, buf, DIAG_HEADER_LENGTH + 5);
	memcpy(rsp + DIAG_HEADER_LENGTH + 5, dut_info, sizeof(DUT_INFO_T));
	rsp_len = DIAG_HEADER_LENGTH + 6 + sizeof(DUT_INFO_T);
	rsp_head->len = rsp_len - 2;
	rsp[rsp_len - 1] = 0x7e;
	free(dut_info);
	close(fd);
	close(fd1);
	close(fd2);
	close(fd3);
	ALOGD("pq_cmd_connect sucess v1\n");
	return rsp_len;
}


int PQTuneCore::tune_rgb_pattern(char *buf, int len, char *rsp, int rsplen)
{
	int fd;
	int fd0;
	int ret = 0;
	char rgb[10] = {0};
	char flip[10] = {0};
	bool exit = false;
	static bool flip_en = true;
	uint32_t *pdata = NULL;
	uint32_t *rsp_pdata = NULL;
	MSG_HEAD_T *rsp_head;
	uint32_t rsp_len = 0;
	uint32_t extra_len = 0;

	fd = open(DispcBg, O_RDWR);
	fd0 = open(FlipDisable, O_RDWR);
	if (fd < 0 || fd0 < 0) {
		ALOGD("%s: open file failed, err: %s\n", __func__, strerror(errno));
		if (fd >= 0)
			close(fd);
		if (fd0 >= 0)
			close(fd0);
		return errno;
	}
	if(flip_en) {
		sprintf(flip, "%d", 1);
		write(fd0, flip, sizeof(flip));
		flip_en = false;
	}
	pdata = (uint32_t *)(buf + DIAG_HEADER_LENGTH + 5);
	rsp_pdata = (uint32_t *)(rsp + DIAG_HEADER_LENGTH + 5);;
	rsp_head = (MSG_HEAD_T *)(rsp + 1);
	if (*pdata != 0xffffffff) {
		sprintf(rgb, "%x", *pdata);
		ret = write(fd, rgb, sizeof(rgb));
		exit = false;
	} else {
		sprintf(flip, "%d", 0);
		write(fd0, flip, sizeof(flip));
		ret = 0;
		exit = true;
		flip_en = true;
	}
	memcpy(rsp, buf, DIAG_HEADER_LENGTH + 5);
	if (ret == -1)
		*rsp_pdata = 1;
	else
		*rsp_pdata = 0;
	rsp_len = DIAG_HEADER_LENGTH + 10;
	rsp_head->len = rsp_len - 2;
	rsp[rsp_len - 1] = 0x7e;
	close(fd);
	close(fd0);
	return rsp_len;

}

int PQTuneCore::tune_read_regs(char *buf, int len, char *rsp, int rsplen)
{
	int fd0;
	int fd1;
	uint32_t *pdata;
	uint32_t offset;
	uint32_t length;
	uint32_t sizes = 0;
	uint32_t rsp_len = 0;
	char cmds[30];
	char *bufs;
	uint32_t extra_len = 0;
	int ret = 0;
	MSG_HEAD_T *rsp_head;

	fd0 = open(DispcRegsOffset, O_RDWR);
	fd1 = open(DispcWrRegs, O_RDWR);
	if (fd0 < 0 || fd1 < 0) {
		ALOGD("%s: open file failed, err: %s\n", __func__, strerror(errno));
		if (fd0 >= 0)
			close(fd0);
		if (fd1 >= 0)
			close(fd1);
		return errno;
	}
	rsp_head = (MSG_HEAD_T *)(rsp + 1);
	pdata = (uint32_t *)(buf + DIAG_HEADER_LENGTH + 5);
	offset = pdata[1];
	length = pdata[0];
	ALOGD("PQ %s: offset = %x len = %x\n", __func__, offset, length);
	bufs = (char *)(rsp + DIAG_HEADER_LENGTH + 13);
	sizes = sprintf(cmds, "%8x ", offset);
	ALOGD(cmds + sizes, "%8x ", length);
	ret = write(fd0 ,cmds, sizeof(cmds));
	ret = read(fd1, bufs, 9 * length);
	HexStrstoInt(bufs, length);
	memcpy(rsp, buf, DIAG_HEADER_LENGTH + 5);
	rsp_len = DIAG_HEADER_LENGTH + 14 + length*4;
	rsp_head->len = rsp_len - 2;
	rsp[rsp_len - 1] = 0x7e;
	close(fd0);
	close(fd1);
	return rsp_len;
}

int PQTuneCore::tune_write_regs(char *buf, int len, char *rsp, int rsplen)
{
	int fd0;
	int fd1;
	uint32_t *pcmd;
	int offset;
	int length;
	uint32_t rsp_len = 0;
	char *cmds;
	int sizes = 0;
	int data_len;
	int *pdata;
	int i = 0;
	int err_rsp = 0;
	int ret = 0;
	uint32_t extra_len = 0;
	MSG_HEAD_T *rsp_head;
	MSG_HEAD_T *send_head;

	fd0 = open(DispcRegsOffset, O_RDWR);
	fd1 = open(DispcWrRegs, O_RDWR);
	if ((fd0 < 0) || (fd1 < 0)) {
		ALOGD("%s: open file failed, err: %s\n", __func__, strerror(errno));
		if (fd0 >= 0)
			close(fd0);
		if (fd1 >= 0)
			close(fd1);
		return errno;
	}
	cmds = (rsp + DIAG_HEADER_LENGTH + 5);
	pcmd = (uint32_t *)(buf + DIAG_HEADER_LENGTH + 5);
	offset = pcmd[1];
	length = pcmd[0];
	printf("PQ %s  offset = %x, length = %x \n", __func__, offset, length);
	sizes = sprintf(cmds, "%8x ", offset);
	sizes += sprintf(cmds + sizes, "%8x ", length);
	rsp_head = (MSG_HEAD_T *)(rsp + 1);
	send_head = (MSG_HEAD_T *)(buf + 1);
	data_len = length;//send_head->len - 12 - DIAG_HEADER_LENGTH;
	ALOGD("PQ %s data_len = %x \n", __func__, data_len);
	pdata = (int*)(buf + DIAG_HEADER_LENGTH + 13);
	ALOGD("PQ %s pdata = %x \n", __func__, *pdata);
	ret = write(fd0 ,cmds, sizes);
	sizes = 0;
	for(i = 0; i < data_len; i++){
		sizes += sprintf(cmds + sizes, "%8x ", *pdata);
		pdata++;
	}
	ret = write(fd1, cmds, sizes);
	rsp[0] = 0x7e;
	memcpy(rsp + DIAG_HEADER_LENGTH + 5, &err_rsp, 4);
	memcpy(rsp + 1, buf + 1, DIAG_HEADER_LENGTH + 2);
	rsp_len = DIAG_HEADER_LENGTH + 8 + 2;
	rsp_head->len = rsp_len - 2;
	rsp[rsp_len - 1] = 0x7e;
	close(fd0);
	close(fd1);

	return rsp_len;
}

int PQTuneCore::tune_start_read_cfg(char *buf, int len, char *rsp, int rsplen)
{
	MSG_HEAD_T *rsp_head;
	uint32_t rsp_len = 0;
	uint32_t sizes = 0;
	uint32_t extra_len = 0;

	rsp_head = (MSG_HEAD_T *)(rsp + 1);
	abc->parse_xml(ctx);
	cms->parse_xml(ctx);
	bld->parse_xml(ctx);
	gamma->parse_xml(ctx);
	hsv->parse_xml(ctx);
	sizes = tune_sizes;
	ALOGD("%s:  %d\n", __func__, sizes);
	memcpy(rsp, buf, DIAG_HEADER_LENGTH + 5);
	memcpy(rsp + DIAG_HEADER_LENGTH + 5, (char *)&sizes, sizeof(uint32_t));
	rsp_len = DIAG_HEADER_LENGTH + 10;
	rsp_head->len = rsp_len - 2;
	rsp[rsp_len - 1] = 0x7e;

	return rsp_len;
}

int PQTuneCore::tune_midst_read_cfg(char *buf, int len, char *rsp, int rsplen)
{
	char* parambuf;
	uint32_t *sizes;
	uint32_t *offset;
	uint32_t rsp_len = 0;
	short err_rsp = 0;
	int extra_len = 0;
	MSG_HEAD_T *rsp_head;

	rsp_head = (MSG_HEAD_T *)(rsp + 1);
	sizes = (uint32_t *)(buf + DIAG_HEADER_LENGTH + 5);
	offset = (uint32_t *)(buf + DIAG_HEADER_LENGTH + 9);
	parambuf = (char *)ctx;
	parambuf += *offset;
	memcpy(rsp, buf, DIAG_HEADER_LENGTH + 5);
	memcpy(rsp + DIAG_HEADER_LENGTH + 7, parambuf, *sizes);
	memcpy(rsp + DIAG_HEADER_LENGTH + 5, &err_rsp, 2);
	rsp_len = DIAG_HEADER_LENGTH + 8 + (*sizes);
	rsp_head->len = rsp_len - 2;
	rsp[rsp_len - 1] = 0x7e;

	return rsp_len;
}

int PQTuneCore::tune_end_read_cfg(char *buf, int len, char *rsp, int rsplen)
{
	uint32_t rsp_len = 0;
	uint32_t err_rsp = 0;
	MSG_HEAD_T *rsp_head;
	uint32_t extra_len = 0;

	rsp_head = (MSG_HEAD_T *)(rsp + 1);
	memcpy(rsp, buf, DIAG_HEADER_LENGTH + 5);
	memcpy(rsp + DIAG_HEADER_LENGTH + 5, &err_rsp, 4);
	rsp_len = DIAG_HEADER_LENGTH + 10;
	rsp_head->len = rsp_len - 2;
	rsp[rsp_len - 1] = 0x7e;

	return rsp_len;
}

int PQTuneCore::tune_start_write_cfg(char *buf, int len, char *rsp, int rsplen)
{
	uint32_t *TuneSizes;
	MSG_HEAD_T *rsp_head;
	uint32_t rsp_len = 0;
	uint32_t sizes = 0;
	uint32_t err_rsp = 0;

	TuneSizes = (uint32_t *)(buf + DIAG_HEADER_LENGTH + 5);
	offset = 0;
	sizes = tune_sizes;
	printf("%s:  %d  TuneSizes %d\n", __func__, sizes, *TuneSizes);
	if(*TuneSizes != sizes)
		err_rsp	 = 1;
	printf("%s:  %d  TuneSizes %d err %d\n", __func__, sizes, *TuneSizes, err_rsp);
	rsp_head = (MSG_HEAD_T *)(rsp + 1);
	memcpy(rsp, buf, DIAG_HEADER_LENGTH + 5);
	memcpy(rsp + DIAG_HEADER_LENGTH + 5, &err_rsp, 4);
	rsp_len = DIAG_HEADER_LENGTH + 10;
	rsp_head->len = rsp_len - 2;
	rsp[rsp_len - 1] = 0x7e;

	return rsp_len;
}

int PQTuneCore::tune_midst_write_cfg(char *buf, int len, char *rsp, int rsplen)
{
	char *pdata;
	char* parambuf;
	MSG_HEAD_T *rsp_head;
	MSG_HEAD_T *send_head;
	uint32_t rsp_len = 0;
	uint32_t sizes;
	uint32_t err = 0;

	parambuf = (char *)ctx;
	parambuf += offset;
	rsp_head = (MSG_HEAD_T *)(rsp + 1);
	send_head = (MSG_HEAD_T *)(buf + 1);
	sizes = send_head->len - DIAG_HEADER_LENGTH - 4;
	pdata = (char *)(buf + DIAG_HEADER_LENGTH + 5);
	memcpy(parambuf, pdata, sizes);
	offset += sizes;
	memcpy(rsp ,buf, DIAG_HEADER_LENGTH + 5);
	memcpy(rsp + DIAG_HEADER_LENGTH + 5, &err, 4);
	rsp_len = DIAG_HEADER_LENGTH + 10;
    rsp_head->len = rsp_len - 2;
	rsp[rsp_len - 1] = 0x7e;

	return rsp_len;
}

int PQTuneCore::tune_end_write_cfg(char *buf, int len, char *rsp, int rsplen)
{
	MSG_HEAD_T *rsp_head;
	uint32_t rsp_len = 0;
	uint32_t err = 0;

	if(offset == tune_sizes) {
		abc->update_xml(ctx);
		cms->update_xml(ctx);
		bld->update_xml(ctx);
		gamma->update_xml(ctx);
		hsv->update_xml(ctx);
		err = 0;
	} else
		err = 1;
	printf("%s: offset %d  TuneSizes %d\n", __func__, offset, err);
	rsp_head = (MSG_HEAD_T *)(rsp + 1);
	memcpy(rsp, buf, DIAG_HEADER_LENGTH + 5);
	memcpy(rsp + DIAG_HEADER_LENGTH + 5, &err, 4);
	rsp_len = DIAG_HEADER_LENGTH + 10;
	rsp_head->len = rsp_len - 2;
	rsp[rsp_len - 1] = 0x7e;

	return rsp_len;
}

static bool parse_pq_module_type(uint16_t cmd){

	switch(cmd) {
		case PQ_VERSION:
		case PQ_GAMMA:
		case PQ_BLD:
		case PQ_CMS:
		case PQ_ABC:
		case PQ_HSV:
			return true;
		default:
			return false;
	}
}

int PQTuneCore::tune_rd_tuning_reg(char *buf, int len, char *rsp, int rsplen)
{

	uint32_t rsp_len = 0;
	uint32_t sizes = 0;
	uint16_t *sub_cmd = 0;
	uint08_t *pdata;
	int ret = 0;
	int extra_len = 0;
	MSG_HEAD_T *rsp_head;

	rsp_head = (MSG_HEAD_T *)(rsp + 1);
	sub_cmd = (uint16_t *)(buf + DIAG_HEADER_LENGTH + 5);
	pdata = (uint08_t *)(rsp + DIAG_HEADER_LENGTH + 7);

	if(!parse_pq_module_type(*sub_cmd))
		goto ERR0;

	switch(*sub_cmd) {
		case PQ_VERSION:
			break;
		case PQ_GAMMA:
			gamma->parse_reg(ctx);
			sizes = gamma_size;
			memcpy(pdata, ctx + sizeof(mainversion), sizes);
			break;
		case PQ_BLD:
			bld->parse_reg(ctx);
			sizes = bld_size;;
			memcpy(pdata, ctx + gamma_size + sizeof(mainversion), sizes);
			break;
		case PQ_CMS:
			cms->parse_reg(ctx);
			sizes = cms_size;
			memcpy(pdata, ctx + gamma_size + bld_size + sizeof(mainversion), sizes);
			break;
		case PQ_ABC:
			abc->parse_reg(ctx);
			sizes = abc_size;
			memcpy(pdata, ctx + gamma_size + bld_size + cms_size + sizeof(mainversion), sizes);
			break;
		case PQ_HSV:
			hsv->parse_reg(ctx);
			sizes = hsv_size;
			memcpy(pdata, ctx + gamma_size + bld_size + cms_size + abc_size + sizeof(mainversion), sizes);
			break;
		default:
			break;
	}

ERR0:
	memcpy(rsp, buf, DIAG_HEADER_LENGTH + 7);
	rsp_len = DIAG_HEADER_LENGTH + sizes + 8;
	rsp_head->len = rsp_len - 2;
	rsp[rsp_len -1] = 0x7e;

	return rsp_len;
}

int PQTuneCore::tune_wr_tuning_reg(char *buf, int len, char *rsp, int rsplen)
{

	uint32_t rsp_len = 0;
	uint32_t sizes = 0;
	uint16_t *sub_cmd = 0;
	uint08_t *pdata;
	uint32_t *perr;
	uint32_t err = 0;
	int ret = 0;
	MSG_HEAD_T *rsp_head;

	rsp_head = (MSG_HEAD_T *)(rsp + 1);
	sub_cmd = (uint16_t *)(buf + DIAG_HEADER_LENGTH + 5);
	pdata = (uint08_t *)(buf + DIAG_HEADER_LENGTH + 7);
	perr = (uint32_t *)(rsp + DIAG_HEADER_LENGTH + 5);

	if(!parse_pq_module_type(*sub_cmd)){
		err = 1;
		goto ERR0;
	}

	switch(*sub_cmd) {
		case PQ_VERSION:
			break;
		case PQ_GAMMA:
			sizes = gamma_size;
			memcpy(ctx + sizeof(mainversion), pdata, sizes);
			gamma->update_reg(ctx);
			break;
		case PQ_BLD:
			sizes = bld_size;
			memcpy(ctx + gamma_size + sizeof(mainversion), pdata, sizes);
			bld->update_reg(ctx);
			break;
		case PQ_CMS:
			sizes = cms_size;
			memcpy(ctx + gamma_size + bld_size + sizeof(mainversion), pdata, sizes);
			cms->update_reg(ctx);
			break;
		case PQ_ABC:
			sizes = abc_size;
			memcpy(ctx + gamma_size + bld_size + cms_size + sizeof(mainversion), pdata, sizes);
			abc->update_reg(ctx);
			break;
		case PQ_HSV:
			sizes = hsv_size;
			memcpy(ctx + gamma_size + bld_size + cms_size + abc_size + sizeof(mainversion), pdata, sizes);
			hsv->update_reg(ctx);
			break;
		default:
			break;
	}

ERR0:
	*perr = err;
	memcpy(rsp, buf, DIAG_HEADER_LENGTH + 5);
	rsp_len = DIAG_HEADER_LENGTH + 10;
	rsp_head->len = rsp_len - 2;
	rsp[rsp_len -1] = 0x7e;

	return rsp_len;

}

int PQTuneCore::tune_rd_tuning_xml(char *buf, int len, char *rsp, int rsplen)
{

	uint32_t rsp_len = 0;
	uint32_t sizes = 0;
	uint16_t* sub_cmd = 0;
	uint32_t *pdata;
	int ret = 0;
	int err = 0;
	int extra_len = 0;
	MSG_HEAD_T *rsp_head;

	rsp_head = (MSG_HEAD_T *)(rsp + 1);
	sub_cmd = (uint16_t *)(buf + DIAG_HEADER_LENGTH + 5);
	pdata = (uint32_t *)(rsp + DIAG_HEADER_LENGTH + 7);

	if(!parse_pq_module_type(*sub_cmd))
		goto ERR0;


	switch(*sub_cmd) {
		case PQ_VERSION:
			break;
		case PQ_GAMMA:
			gamma->parse_xml(ctx);
			sizes = gamma_size;
			memcpy(pdata, ctx + sizeof(mainversion), sizes);
			break;
		case PQ_BLD:
			bld->parse_xml(ctx);
			sizes = bld_size;
			memcpy(pdata, ctx + gamma_size + sizeof(mainversion), sizes);
			break;
		case PQ_CMS:
			cms->parse_xml(ctx);
			sizes = cms_size;
			memcpy(pdata, ctx + gamma_size + bld_size + sizeof(mainversion), sizes);
			break;
		case PQ_ABC:
			abc->parse_xml(ctx);
			sizes = abc_size;
			memcpy(pdata, ctx + gamma_size + bld_size + cms_size + sizeof(mainversion), sizes);
			break;
		case PQ_HSV:
			hsv->parse_xml(ctx);
			sizes = hsv_size;
			memcpy(pdata, ctx + gamma_size + bld_size + cms_size + abc_size + sizeof(mainversion), sizes);
			break;
		default:
			break;
	}

ERR0:
	memcpy(rsp, buf, DIAG_HEADER_LENGTH +  7);
	rsp_len = DIAG_HEADER_LENGTH + sizes + 8;
	rsp_head->len = rsp_len - 2;
	rsp[rsp_len -1] = 0x7e;

	return rsp_len;
}

int PQTuneCore::tune_wr_tuning_xml(char *buf, int len, char *rsp, int rsplen)
{

	uint32_t rsp_len = 0;
	uint32_t sizes = 0;
	uint16_t* sub_cmd = 0;
	uint08_t* pdata;
	uint32_t *perr;
	uint32_t err = 0;
	int ret = 0;
	MSG_HEAD_T *rsp_head;

	rsp_head = (MSG_HEAD_T *)(rsp + 1);
	sub_cmd = (uint16_t *)(buf + DIAG_HEADER_LENGTH + 5);
	pdata = (uint08_t *)(buf + DIAG_HEADER_LENGTH + 7);
	perr = (uint32_t *)(rsp + DIAG_HEADER_LENGTH + 5);

	if(!parse_pq_module_type(*sub_cmd)){
		err = 1;
		goto ERR0;
	}

	switch(*sub_cmd) {
		case PQ_VERSION:
			break;
		case PQ_GAMMA:
			sizes = gamma_size;
			memcpy(ctx + sizeof(mainversion), pdata, sizes);
			gamma->update_xml(ctx);
			break;
		case PQ_BLD:
			sizes = bld_size;
			memcpy(ctx + gamma_size + sizeof(mainversion), pdata, sizes);
			bld->update_xml(ctx);
			break;
		case PQ_CMS:
			sizes = cms_size;
			memcpy(ctx + gamma_size + bld_size + sizeof(mainversion), pdata, sizes);
			cms->update_xml(ctx);

			break;
		case PQ_ABC:
			sizes = abc_size;
			memcpy(ctx + gamma_size + bld_size + cms_size + sizeof(mainversion), pdata, sizes);
			abc->update_xml(ctx);
			break;
		case PQ_HSV:
			sizes = hsv_size;
			memcpy(ctx + gamma_size + bld_size + cms_size + abc_size + sizeof(mainversion), pdata, sizes);
			hsv->update_xml(ctx);
			break;
		default:
			break;
	}
ERR0:
	*perr = err;
	memcpy(rsp, buf, DIAG_HEADER_LENGTH + 5);
	rsp_len = DIAG_HEADER_LENGTH + 10;
	rsp_head->len = rsp_len - 2;
	rsp[rsp_len -1] = 0x7e;

	return rsp_len;

}

int PQTuneCore::tune_rd_ambient(char *buf, int len, char *rsp, int rsplen)
{
	uint32_t *pdata = NULL;
	MSG_HEAD_T *rsp_head;
	uint32_t rsp_len = 0;
	uint32_t value = 0;
	int ret;
	int fd;
	long long tvalue;
	long long *tfpvalue;
	int extra_len = 0;
	char tbuf[256];


	fd = open(SensorLight, O_RDONLY);
	if (fd < 0) {
		printf("%s: open file failed, err: %s\n", __func__,
			strerror(errno));
		return errno;
	}

	ret = read(fd, tbuf, sizeof(tbuf));
	if (ret < 0) {
		printf("PQ read Ambient Light value Fail\n");
		close(fd);
		return -1;
	} else if (ret == 0) {
			 ALOGD("PQ read Ambient Light ret 0\n");
	} else {
			ALOGD("PQ read %d bytes, buf = %s\n", ret, tbuf);
			tvalue = atoll(tbuf);
			ALOGD("PQ tval = %llu\n", tvalue);
			tfpvalue = &tvalue;
			ALOGD("PQ float tval = %f\n", *tfpvalue);
			value = (uint32_t)*tfpvalue;
			ALOGD("PQ int val = %d\n", value);
	}
	rsp_head = (MSG_HEAD_T *)(rsp + 1);
	pdata = (uint32_t *)(rsp + DIAG_HEADER_LENGTH + 5);
	*pdata = value;
	memcpy(rsp, buf, DIAG_HEADER_LENGTH + 5);
	rsp_len = DIAG_HEADER_LENGTH + 10;
	rsp_head->len = rsp_len - 2;
	rsp[rsp_len - 1] = 0x7e;
	close(fd);

	return rsp_len;
}

