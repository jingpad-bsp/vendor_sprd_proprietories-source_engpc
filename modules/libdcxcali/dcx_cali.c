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
#include "sprd_fts_log.h"
#include "sprd_fts_diag.h"

#include "dcx_cali.h"
#include "sc2730_glb.h"

typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;
#define BIT(x) (1<<x)
int current_mode = 1;
static u32 hp_cbank =0, lp_cbank =0, hp_amp =0, lp_amp =0;

#define PMIC_GLB_VALUE "/sys/bus/platform/drivers/sprd-pmic-glb/sc27xx-syscon/pmic_value"
#define PMIC_GLB_REG "/sys/bus/platform/drivers/sprd-pmic-glb/sc27xx-syscon/pmic_reg"

static u32 ana_read(u32 reg_addr)
{
    int fd_reg, fd_val, ret;
    char cmds[30] = {0}, ret_val[30] = {0};

    fd_val = open(PMIC_GLB_VALUE, O_RDWR);
    fd_reg = open(PMIC_GLB_REG, O_RDWR);
    sprintf(cmds,"%x",reg_addr);
    ret = write( fd_reg, cmds, sizeof(cmds));
    ret = read( fd_val, ret_val,sizeof(ret_val));
    ENG_LOG("%s: %d ctrl5:0x cmds:%s, ret_val:%s",__FUNCTION__,__LINE__,cmds,ret_val);
    close(fd_reg);
    close(fd_val);
    return (u32)strtol(ret_val,NULL,16);
}

static u32 ana_write(u32 reg_addr,u32 value)
{
    int fd_reg, fd_val, ret;
    char cmds_addr[30] = {0}, cmds_value[30] = {0};
    fd_val = open(PMIC_GLB_VALUE, O_RDWR);
    fd_reg = open(PMIC_GLB_REG, O_RDWR);
    sprintf(cmds_addr,"%8x", reg_addr);
    write( fd_reg, cmds_addr, sizeof(cmds_addr));
    sprintf(cmds_value,"%8x", value);
    write( fd_val, cmds_value, sizeof(cmds_value));
    ENG_LOG("%s: %d ctrl5:0x, reg_addr:0x%x val:0x%x",__FUNCTION__,__LINE__,reg_addr,value);
    close(fd_reg);
    close(fd_val);
    usleep(10000);
    return ret;
}

static u32 hp_dcxo_amp_init()
{
    u32 val;
    val = ana_read(ANA_REG_GLB_TSX_CTRL3);
    ana_write(ANA_REG_GLB_TSX_CTRL3, val | BIT_DCXO_CORE_AML_CAL_EN);
    val = ana_read(ANA_REG_GLB_TSX_CTRL5);
    ana_write(ANA_REG_GLB_TSX_CTRL5,val|BIT_DCXO_CORE_AML_CAL_CTRL_HP(0x1F));
    val = ana_read(ANA_REG_GLB_TSX_CTRL5);
    ana_write(ANA_REG_GLB_TSX_CTRL5,val|BIT_DCXO_CORE_AML_CAL_CTRL_HP(0xE0));
    val = ana_read(ANA_REG_GLB_TSX_CTRL4);
    ana_write(ANA_REG_GLB_TSX_CTRL4,val|BIT_DCXO_CORE_AML_CTRL(0x7));

    return 0;
}

#define OK_FLAG (ana_read(ANA_REG_GLB_TSX_CTRL4) & BIT_DCXO_CORE_AML_CAL_OK_FLAG)
#define HP_CAL_CTRL_7_5 ((ana_read(ANA_REG_GLB_TSX_CTRL5) & BIT_DCXO_CORE_AML_CAL_CTRL_HP(0xE0)) >> 5) //bit[7:5]
#define HP_CAL_CTRL_4_0 (ana_read(ANA_REG_GLB_TSX_CTRL5) & BIT_DCXO_CORE_AML_CAL_CTRL_HP(0x1F)) //bit[7:5]

static u32 hp_amp_cali()
{
    u32 val;
    ana_write(ANA_REG_GLB_TSX_WR_PROT_VALUE,0x1990);
    hp_dcxo_amp_init();

    while( !(OK_FLAG !=0 ) && !(OK_FLAG == 0 && HP_CAL_CTRL_7_5 == 0)) {
        val = HP_CAL_CTRL_7_5 - 1;
	val = BIT_DCXO_CORE_AML_CAL_CTRL_HP((ana_read(ANA_REG_GLB_TSX_CTRL5) & 0x1F) | (val << 5));
        ana_write(ANA_REG_GLB_TSX_CTRL5,val);
    }

    if (OK_FLAG !=0 && HP_CAL_CTRL_7_5 != 0) {
        val = HP_CAL_CTRL_7_5 + 1;
        ana_write(ANA_REG_GLB_TSX_CTRL5,
                    BIT_DCXO_CORE_AML_CAL_CTRL_HP((ana_read(ANA_REG_GLB_TSX_CTRL5) & ~0xE0) | val << 5));
    }

    if (OK_FLAG == 0 && HP_CAL_CTRL_7_5 == 0) {
        ana_write(ANA_REG_GLB_TSX_CTRL5,ana_read(ANA_REG_GLB_TSX_CTRL5) && 0x1F);
    }

    ana_write(ANA_REG_GLB_TSX_CTRL4,(ana_read(ANA_REG_GLB_TSX_CTRL4)& ~BIT_DCXO_CORE_AML_CTRL(0XF))
                | BIT_DCXO_CORE_AML_CTRL(0x6));
    ana_write(ANA_REG_GLB_TSX_CTRL5,ana_read(ANA_REG_GLB_TSX_CTRL5) | 0x1F);

    while(OK_FLAG != BIT_DCXO_CORE_AML_CAL_OK_FLAG ) {
        val = HP_CAL_CTRL_4_0 - 1;
        ana_write(ANA_REG_GLB_TSX_CTRL5,(ana_read(ANA_REG_GLB_TSX_CTRL5) & ~0x1F) | val);
	usleep(1000);
    }

    ana_write(ANA_REG_GLB_TSX_CTRL3, ana_read(ANA_REG_GLB_TSX_CTRL3) & ~BIT_DCXO_CORE_AML_CAL_EN);


    return ana_read(ANA_REG_GLB_TSX_CTRL5);
}

static u32 lp_dcxo_amp_init()
{
    u32 val;
    val = ana_read(ANA_REG_GLB_TSX_CTRL3);
    ana_write(ANA_REG_GLB_TSX_CTRL3, val | BIT_DCXO_CORE_AML_CAL_EN);
    val = ana_read(ANA_REG_GLB_TSX_CTRL5);
    ana_write(ANA_REG_GLB_TSX_CTRL5,val|BIT_DCXO_CORE_AML_CAL_CTRL_LP(0xF));
    val = ana_read(ANA_REG_GLB_TSX_CTRL5);
    ana_write(ANA_REG_GLB_TSX_CTRL5,val|BIT_DCXO_CORE_AML_CAL_CTRL_LP(0x30));
    val = ana_read(ANA_REG_GLB_TSX_CTRL4);
    ana_write(ANA_REG_GLB_TSX_CTRL4,val|BIT_DCXO_CORE_AML_CTRL(0x2));

    return 0;
}

#define LP_CAL_CTRL_5_4 ((ana_read(ANA_REG_GLB_TSX_CTRL5) & BIT_DCXO_CORE_AML_CAL_CTRL_LP(0x30)) >> 4) //bit[7:4]
#define LP_CAL_CTRL_3_0 (ana_read(ANA_REG_GLB_TSX_CTRL5) & BIT_DCXO_CORE_AML_CAL_CTRL_LP(0xF)) //bit[3:0]

static u32 lp_amp_cali()
{
    u32 val,cbank_bak;
    ana_write(ANA_REG_GLB_TSX_WR_PROT_VALUE,0x1990);
    cbank_bak = ana_read(ANA_REG_GLB_TSX_CTRL10);
    ana_write(ANA_REG_GLB_TSX_CTRL10,0x0);
    lp_dcxo_amp_init();

    while( !(OK_FLAG !=0 ) &&
        !(OK_FLAG == 0 && LP_CAL_CTRL_5_4 == 0)) {
            val = LP_CAL_CTRL_5_4 - 1;
            ana_write(ANA_REG_GLB_TSX_CTRL5,
                        BIT_DCXO_CORE_AML_CAL_CTRL_LP((ana_read(ANA_REG_GLB_TSX_CTRL5) & ~0xF0) | val << 4));
    }

    if (OK_FLAG !=0 && LP_CAL_CTRL_5_4 != 0) {
        val =  + 1;
        ana_write(ANA_REG_GLB_TSX_CTRL5,
                    BIT_DCXO_CORE_AML_CAL_CTRL_LP((ana_read(ANA_REG_GLB_TSX_CTRL5) & ~0xF0) | val << 4));
    }

    if (OK_FLAG == 0 && LP_CAL_CTRL_5_4 == 0) {
        ana_write(ANA_REG_GLB_TSX_CTRL5,ana_read(ANA_REG_GLB_TSX_CTRL5) && 0xF);
    }

        ana_write(ANA_REG_GLB_TSX_CTRL4,(ana_read(ANA_REG_GLB_TSX_CTRL4)& ~BIT_DCXO_CORE_AML_CTRL(0XF))
                    | BIT_DCXO_CORE_AML_CTRL(0x1));
        ana_write(ANA_REG_GLB_TSX_CTRL5,ana_read(ANA_REG_GLB_TSX_CTRL5) | 0xF);

    while(OK_FLAG != BIT_DCXO_CORE_AML_CAL_OK_FLAG) {
        val = LP_CAL_CTRL_3_0 - 1;
        ana_write(ANA_REG_GLB_TSX_CTRL5,(ana_read(ANA_REG_GLB_TSX_CTRL5) & ~0xF) | val);
    }

    ana_write(ANA_REG_GLB_TSX_CTRL3, ana_read(ANA_REG_GLB_TSX_CTRL3) & ~BIT_DCXO_CORE_AML_CAL_EN);

    ana_write(ANA_REG_GLB_TSX_CTRL10,cbank_bak);
    return ana_read(ANA_REG_GLB_TSX_CTRL5);
}

#define ENG_RAWDATA_FILE "/mnt/vendor/productinfo/dcx_data.dat"

int write_dcx_data(char *data, int nLen)
{
    int rcount;
    int ret = 0, fd = -1;
    FILE * fp = NULL;
    if(NULL == data)
    {
        ENG_LOG("%s: req is NULL!!!",__FUNCTION__);
        ret = -1;
        return ret;
    }

    ENG_LOG("%s: %s exists",__FUNCTION__, ENG_RAWDATA_FILE);
    fp = fopen(ENG_RAWDATA_FILE, "w+");
    if(NULL == fp)
    {
        ENG_LOG("%s: fopen fail errno=%d, strerror(errno)=%s",__FUNCTION__, errno, strerror(errno));
        ret = -1;
        return ret;
    }
    rcount = fwrite(data, sizeof(char), nLen, fp);
    ENG_LOG("%s: fwrite count %d",__FUNCTION__, rcount);
    if(nLen != rcount)
    {
        ENG_LOG("%s: rcount is not matched!",__FUNCTION__);
        ret = -1;
    }
    else
    {
        fflush(fp);
        fd = fileno(fp);
        if(fd > 0) {
            fsync(fd);
            ret = rcount;
        } else {
            ENG_LOG("%s: fileno() error, strerror(errno)=%s", __FUNCTION__, strerror(errno));
            ret = -1;
        }
    }

    fclose(fp);
    return ret;
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
  }else{
    ret = -1;
    ENG_LOG("%s: %s not exists",__FUNCTION__, ENG_RAWDATA_FILE);
    return ret;
  }
  return rcount;
}

static int dcx_set_afc_mode(char *buf, int len, char *rsp, int rsplen)
{
    int ret = 0;
    MSG_HEAD_T* msg_head_ptr = NULL;
    DIAG_AP_PMIC_AFC_MODE* lpPmicAfcMode = NULL;
    TOOLS_DIAG_AP_CNF_T* lpDiagApCnf = NULL;
    u32 *pdata;
    u32 reg_val;
    DIAG_AP_PMIC_AFC_MODE *afc_mode;

    if(NULL == buf || NULL == rsp){
        ENG_LOG("%s,null pointer",__FUNCTION__);
        return 0;
    }

    memcpy(rsp, buf, sizeof(MSG_HEAD_T)+1);
    msg_head_ptr = (MSG_HEAD_T*)(rsp+1);
    lpDiagApCnf = (TOOLS_DIAG_AP_CNF_T*)(rsp+sizeof(MSG_HEAD_T)+1);
    msg_head_ptr->len = sizeof(MSG_HEAD_T)+sizeof(TOOLS_DIAG_AP_CNF_T);

    pdata = (buf + 1 + sizeof(MSG_HEAD_T) + sizeof(TOOLS_DIAG_AP_CMD_T));
    //ENG_LOG("%s: mode = %x, reserved = %x",__FUNCTION__, pdata->mode, pdata->reserved);

    afc_mode = pdata;
    ENG_LOG("%s: afc_mode = %x reg:0x%x\n", __func__, afc_mode->mode, afc_mode->reserved);

    ana_write(ANA_REG_GLB_TSX_WR_PROT_VALUE,0x1990);
    if (afc_mode->mode == 0 ) {// 0 lowpower mode
        if (current_mode == 1)
        {
		hp_cbank = ana_read(ANA_REG_GLB_TSX_CTRL10) & BIT_DCXO_CORE_CBANK_HP(0xFF);
		hp_amp = ana_read(ANA_REG_GLB_TSX_CTRL5);
		ana_write(ANA_REG_GLB_TSX_CTRL10,0x0);
		ana_write(ANA_REG_GLB_TSX_CTRL14,
			  ana_read(ANA_REG_GLB_TSX_CTRL14) | BIT_DCXO_LP_CAL_EN);
        }
        current_mode = 0;
    }else { //1 high power mode
        /*check with asic how to switch to normal mode*/
        if (current_mode == 0) {
		lp_cbank = ana_read(ANA_REG_GLB_TSX_CTRL10);
		lp_amp = ana_read(ANA_REG_GLB_TSX_CTRL5);
		ana_write(ANA_REG_GLB_TSX_CTRL14,
		ana_read(ANA_REG_GLB_TSX_CTRL14) & ~BIT_DCXO_LP_CAL_EN);
		ana_write(ANA_REG_GLB_TSX_CTRL10,hp_cbank);
		ana_write(ANA_REG_GLB_TSX_CTRL5,hp_amp);
	}
        current_mode = 1;
    }

    if (afc_mode->mode == 0) {//low mode
        lpDiagApCnf->status = 0;
        lpDiagApCnf->length = 0;
    }else if(afc_mode->mode == 1) {// high mode
        lpDiagApCnf->status = 0;
        lpDiagApCnf->length = 0;
    }else{
        lpDiagApCnf->status = 1;
        lpDiagApCnf->length = 0;
    }

    *((char *)lpDiagApCnf+sizeof(TOOLS_DIAG_AP_CNF_T)) = 0x7E;

    return msg_head_ptr->len+2;
}

static int dcx_set_afc_cdac(char *buf, int len, char *rsp, int rsplen)
{
    int ret = 0;
    MSG_HEAD_T* msg_head_ptr = NULL;
    DIAG_AP_SET_PMIC_CDAC* lpPmicCdac = NULL;
    TOOLS_DIAG_AP_CNF_T* lpDiagApCnf = NULL;
    u32 *pdata;
    DIAG_AP_SET_PMIC_CDAC *cdac_data;

    if(NULL == buf || NULL == rsp){
        ENG_LOG("%s,null pointer",__FUNCTION__);
        return 0;
    }

    memcpy(rsp, buf, sizeof(MSG_HEAD_T)+1);
    msg_head_ptr = (MSG_HEAD_T*)(rsp+1);
    lpDiagApCnf = (TOOLS_DIAG_AP_CNF_T*)(rsp+sizeof(MSG_HEAD_T)+1);
    msg_head_ptr->len = sizeof(MSG_HEAD_T)+sizeof(TOOLS_DIAG_AP_CNF_T);

    pdata = (buf + 1 + sizeof(MSG_HEAD_T) + sizeof(TOOLS_DIAG_AP_CMD_T));
    //ENG_LOG("%s: CDAC = %x, reserved = %x",__FUNCTION__, lpPmicCdac->CDAC, lpPmicCdac->reserved);

    /*----------------------------------------------------------------------------*/

    /*receive tool value is char type need transform*/
    cdac_data = pdata;
    ENG_LOG("%s: cdac_data = %x \n", __func__, cdac_data->CDAC);
    ana_write(ANA_REG_GLB_TSX_WR_PROT_VALUE,0x1990);

    if(current_mode == 1)
        ana_write(ANA_REG_GLB_TSX_CTRL10, cdac_data->CDAC);
    else
        ana_write(ANA_REG_GLB_TSX_CTRL11, cdac_data->CDAC);

    /*----------------------------------------------------------------------------*/

    lpDiagApCnf->status = 0;
    lpDiagApCnf->length = 0;

    *((char *)lpDiagApCnf+sizeof(TOOLS_DIAG_AP_CNF_T)) = 0x7E;

    return msg_head_ptr->len+2;
}

static u32 find_amp_cal_finish(int current)
{
    if(current)
       return hp_amp_cali();
    else
       return lp_amp_cali();

}

static int dcx_amp_cali(char *buf, int len, char *rsp, int rsplen)
{
    int ret = 0;
    MSG_HEAD_T* msg_head_ptr = NULL;
    DIAG_AP_PMIC_AFC_AMP_DATA_T* lpAmpData = NULL;
    TOOLS_DIAG_AP_CNF_T* lpDiagApCnf = NULL;
    u32 *pdata;
    u32 reg;

    if(NULL == buf || NULL == rsp){
        ENG_LOG("%s,null pointer",__FUNCTION__);
        return 0;
    }

    memcpy(rsp, buf, sizeof(MSG_HEAD_T)+1);
    msg_head_ptr = (MSG_HEAD_T*)(rsp+1);
    lpDiagApCnf = (TOOLS_DIAG_AP_CNF_T*)(rsp+sizeof(MSG_HEAD_T)+1);
    msg_head_ptr->len = sizeof(MSG_HEAD_T)+sizeof(TOOLS_DIAG_AP_CNF_T)+sizeof(DIAG_AP_PMIC_AFC_AMP_DATA_T);

    lpAmpData = (DIAG_AP_PMIC_AFC_AMP_DATA_T* )(rsp + 1 + sizeof(MSG_HEAD_T) + sizeof(TOOLS_DIAG_AP_CNF_T));
    ENG_LOG("%s: Amp_26M = %x, reserved = %x",__FUNCTION__, lpAmpData->Amp_26M, lpAmpData->reserved);

    /*----------------------------------------------------------------------------*/

    reg = find_amp_cal_finish(current_mode);
    ENG_LOG("%s: reg = %x",__FUNCTION__, reg);
    lpAmpData->Amp_26M = reg;
    lpAmpData->reserved = 0;

    /*----------------------------------------------------------------------------*/

    lpDiagApCnf->status = 0;
    lpDiagApCnf->length = sizeof(DIAG_AP_PMIC_AFC_AMP_DATA_T);
    //lpAmpData->Amp_26M = 0;
    //lpAmpData->reserved = 0;

    *((char *)lpAmpData+sizeof(DIAG_AP_PMIC_AFC_AMP_DATA_T)) = 0x7E;

    return msg_head_ptr->len+2;
}

static int dcx_load_afc_data(char *buf, int len, char *rsp, int rsplen)
{
    int ret = 0;
    MSG_HEAD_T* msg_head_ptr = NULL;
    DIAG_AP_PMIC_AFC_CALI_DATA_T* lpPmicCaliData = NULL;
    TOOLS_DIAG_AP_CNF_T* lpDiagApCnf = NULL;

    if(NULL == buf || NULL == rsp){
        ENG_LOG("%s,null pointer",__FUNCTION__);
        return 0;
    }

    memcpy(rsp, buf, sizeof(MSG_HEAD_T)+1);
    msg_head_ptr = (MSG_HEAD_T*)(rsp+1);
    lpDiagApCnf = (TOOLS_DIAG_AP_CNF_T*)(rsp+sizeof(MSG_HEAD_T)+1);
    msg_head_ptr->len = sizeof(MSG_HEAD_T)+sizeof(TOOLS_DIAG_AP_CNF_T)+sizeof(DIAG_AP_PMIC_AFC_CALI_DATA_T);

    lpPmicCaliData = (DIAG_AP_PMIC_AFC_CALI_DATA_T* )(rsp + 1 + sizeof(MSG_HEAD_T) + sizeof(TOOLS_DIAG_AP_CMD_T));
    ret = read_dcx_data((char *)lpPmicCaliData, sizeof(DIAG_AP_PMIC_AFC_CALI_DATA_T));
    ENG_LOG("%s: ret = %d, CDAC = %x, HP_Mode_Amp_26M = %x, LP_Mode_Amp_26M = %x, LP_Mode_Freq_Drift_32K = %x",__FUNCTION__, ret, 
            lpPmicCaliData->CDAC, lpPmicCaliData->HP_Mode_Amp_26M, lpPmicCaliData->LP_Mode_Amp_26M, lpPmicCaliData->LP_Mode_Freq_Drift_32K);
    lpDiagApCnf->status = (ret==sizeof(DIAG_AP_PMIC_AFC_CALI_DATA_T)?0:1);
    lpDiagApCnf->length = sizeof(DIAG_AP_PMIC_AFC_CALI_DATA_T);

    *((char *)lpPmicCaliData+sizeof(DIAG_AP_PMIC_AFC_CALI_DATA_T)) = 0x7E;

    return msg_head_ptr->len+2;
}

static int dcx_save_afc_data(char *buf, int len, char *rsp, int rsplen)
{
    int ret = 0;
    MSG_HEAD_T* msg_head_ptr = NULL;
    DIAG_AP_PMIC_AFC_CALI_DATA_T* lpPmicCaliData = NULL;
    TOOLS_DIAG_AP_CNF_T* lpDiagApCnf = NULL;

    if(NULL == buf || NULL == rsp){
        ENG_LOG("%s,null pointer",__FUNCTION__);
        return 0;
    }

    memcpy(rsp, buf, sizeof(MSG_HEAD_T)+1);
    msg_head_ptr = (MSG_HEAD_T*)(rsp+1);
    lpDiagApCnf = (TOOLS_DIAG_AP_CNF_T*)(rsp+sizeof(MSG_HEAD_T)+1);
    msg_head_ptr->len = sizeof(MSG_HEAD_T)+sizeof(TOOLS_DIAG_AP_CNF_T);

    lpPmicCaliData = (DIAG_AP_PMIC_AFC_CALI_DATA_T* )(buf + 1 + sizeof(MSG_HEAD_T) + sizeof(TOOLS_DIAG_AP_CMD_T));
    ENG_LOG("%s:  CDAC = %x, HP_Mode_Amp_26M = %x, LP_Mode_Amp_26M = %x, LP_Mode_Freq_Drift_32K = %x",__FUNCTION__,
            lpPmicCaliData->CDAC, lpPmicCaliData->HP_Mode_Amp_26M, lpPmicCaliData->LP_Mode_Amp_26M, lpPmicCaliData->LP_Mode_Freq_Drift_32K);

    ret = write_dcx_data((char *)lpPmicCaliData, sizeof(DIAG_AP_PMIC_AFC_CALI_DATA_T));
    ENG_LOG("%s:  ret = %d",__FUNCTION__, ret);
    lpDiagApCnf->status = (ret==sizeof(DIAG_AP_PMIC_AFC_CALI_DATA_T)?0:1);
    lpDiagApCnf->length = 0;

    *((char *)lpDiagApCnf+sizeof(TOOLS_DIAG_AP_CNF_T)) = 0x7E;

    return msg_head_ptr->len+2;
}

static int dcx_handler(char *buf, int len, char *rsp, int rsplen)
{
    int ret = 0;
    MSG_HEAD_T* msg_head_ptr = NULL;
    TOOLS_DIAG_AP_CMD_T* lpDiagApReq = NULL;

    if(NULL == buf || NULL == rsp){
        ENG_LOG("%s,null pointer",__FUNCTION__);
        return 0;
    }

    lpDiagApReq = (TOOLS_DIAG_AP_CMD_T* )(buf + 1 + sizeof(MSG_HEAD_T));
    switch(lpDiagApReq->cmd){
        case DIAG_AP_CMD_SET_PMIC_AFC_CDAC:
            ret = dcx_set_afc_cdac(buf,len,rsp,rsplen);
            break;
        case DIAG_AP_CMD_PMIC_AFC_AMP_CALI:
            ret = dcx_amp_cali(buf,len,rsp,rsplen);
            break;
        case DIAG_AP_CMD_SET_PMIC_AFC_MODE:
            ret = dcx_set_afc_mode(buf,len,rsp,rsplen);
            break;
        case DIAG_AP_CMD_LOAD_PMIC_AFC_DATA:
            ret = dcx_load_afc_data(buf,len,rsp,rsplen);
            break;
        case DIAG_AP_CMD_SAVE_PMIC_AFC_DATA:
            ret = dcx_save_afc_data(buf,len,rsp,rsplen);
            break;
        default:
            break;
    }

    return ret;
}

void register_this_module_ext(struct eng_callback *reg, int *num)
{
    int moudles_num = 0;
    ENG_LOG("register_this_module_ext :libdcx0cali");

    //1st command
    reg->type = 0x62; //main cmd
    reg->subtype = 0x0; //sub cmd
    //reg->also_need_to_cp = 1;
    reg->diag_ap_cmd = DIAG_AP_CMD_SET_PMIC_AFC_CDAC;
    reg->eng_diag_func = dcx_handler; // rsp function ptr
    moudles_num++;
    reg++;

    reg->type = 0x62; //main cmd
    reg->subtype = 0x0; //sub cmd
    //reg->also_need_to_cp = 1;
    reg->diag_ap_cmd = DIAG_AP_CMD_PMIC_AFC_AMP_CALI;
    reg->eng_diag_func = dcx_handler; // rsp function ptr
    moudles_num++;
    reg++;

    reg->type = 0x62; //main cmd
    reg->subtype = 0x0; //sub cmd
    reg->also_need_to_cp = 1;
    reg->diag_ap_cmd = DIAG_AP_CMD_LOAD_PMIC_AFC_DATA;
    reg->eng_diag_func = dcx_handler; // rsp function ptr
    moudles_num++;
    reg++;

    reg->type = 0x62; //main cmd
    reg->subtype = 0x0; //sub cmd
    reg->also_need_to_cp = 1;
    reg->diag_ap_cmd = DIAG_AP_CMD_SAVE_PMIC_AFC_DATA;
    reg->eng_diag_func = dcx_handler; // rsp function ptr
    moudles_num++;
    reg++;

    reg->type = 0x62; //main cmd
    reg->subtype = 0x0; //sub cmd
    //reg->also_need_to_cp = 1;
    reg->diag_ap_cmd = DIAG_AP_CMD_SET_PMIC_AFC_MODE;
    reg->eng_diag_func = dcx_handler; // rsp function ptr
    moudles_num++;
    reg++;

    *num = moudles_num;
    ENG_LOG("register_this_module_ext: %d - %d",*num, moudles_num);
}
