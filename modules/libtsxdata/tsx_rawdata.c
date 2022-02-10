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
#include "tsx_rawdata.h"

#define ENG_RAWDATA_FILE   "/mnt/vendor/productinfo/wcn/tsx_bt_data.txt"
#define ENG_TXDATA_FILE    "/mnt/vendor/txdata.txt"
#define ENG_TXDATA_BK_FILE    "/mnt/vendor/tsx.dat"

int translate_packet(char *dest, char *src, int size) {

  dest[0] = 0x7E;
  dest++;
  memcpy(dest, src, size);
  dest[size] = 0x7E;

  return size+2;
}

int write_tsx_rawdata(char *data, int nLen)
{
    int rcount;
    int ret = 0, fd = -1;
    FILE * fp = NULL;
    mode_t old_mask;
    static first_flag = 1;
    if(NULL == data)
    {
        ENG_LOG("%s: req is NULL!!!",__FUNCTION__);
        ret = -1;
        return ret;
    }

    if (0 != access("/mnt/vendor/productinfo/wcn", F_OK)) {
        ret = mkdir("/mnt/vendor/productinfo/wcn", S_IRWXU | S_IRWXG | S_IRWXO);
        if (-1 == ret && (errno != EEXIST)) {
            ENG_LOG("mkdir /productinfo/wcn failed.");
            return -1;
        }
    }

    ENG_LOG("%s: %s exists",__FUNCTION__, ENG_RAWDATA_FILE);
    if(first_flag) old_mask = umask(0);
    fp = fopen(ENG_RAWDATA_FILE, "w+");
    if(first_flag) umask(old_mask);
    if(NULL == fp)
    {
        ENG_LOG("%s: fopen fail errno=%d, strerror(errno)=%s",__FUNCTION__, errno, strerror(errno));
        first_flag = 1;
        ret = -1;
        return ret;
    }
    else
    {
        first_flag = 0;
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
        } else {
            ENG_LOG("%s: fileno() error, strerror(errno)=%s", __FUNCTION__, strerror(errno));
            ret = -1;
        }
    }

    fclose(fp);
    return ret;
}

int read_tsx_rawdata(char *buff, int nLen)
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
  }
  return rcount;
}

static int tsx_rawdata_rw_handler(char *buf, int len, char *rsp, int rsplen)
{
    int ret = 0;
    MSG_HEAD_T* msg_head_ptr = NULL;
    DIAG_TSX_DATA_HEAD* src_rawdata_head_ptr = NULL;
    DIAG_TSX_DATA_HEAD* dst_rawdata_head_ptr = NULL;

    if(NULL == buf || NULL == rsp){
        ENG_LOG("%s,null pointer",__FUNCTION__);
        return 0;
    }

    src_rawdata_head_ptr = (DIAG_TSX_DATA_HEAD *)(buf + 1 + sizeof(MSG_HEAD_T));
    memset(rsp, 0, rsplen);
    msg_head_ptr = (MSG_HEAD_T*)(rsp + 1);
    dst_rawdata_head_ptr = (DIAG_TSX_DATA_HEAD *)(rsp + 1 + sizeof(MSG_HEAD_T));
    memcpy(rsp, buf, sizeof(MSG_HEAD_T)+sizeof(DIAG_TSX_DATA_HEAD)+1);

    if(0 == src_rawdata_head_ptr->subcmd)
    {
        int nLen = src_rawdata_head_ptr->datalen;

        ret = write_tsx_rawdata((char*)src_rawdata_head_ptr+sizeof(DIAG_TSX_DATA_HEAD), nLen);
        ENG_LOG("status=%d ret=%d",dst_rawdata_head_ptr->status, ret);
        if(0 == ret)
        {
            dst_rawdata_head_ptr->status = 0;
        }else{
            dst_rawdata_head_ptr->status = 1;
        }
        msg_head_ptr->len = sizeof(MSG_HEAD_T)+sizeof(DIAG_TSX_DATA_HEAD);
        dst_rawdata_head_ptr->datalen = 0;
        ((char *)(dst_rawdata_head_ptr))[sizeof(DIAG_TSX_DATA_HEAD)] = 0x7e;
        ENG_LOG("status=%d",dst_rawdata_head_ptr->status);
    }else if(1 == src_rawdata_head_ptr->subcmd){
        char rawdata[256] = {0};
        ret = read_tsx_rawdata(rawdata, sizeof(rawdata));
        ENG_LOG("*******************ret = %d", ret);

        if( ret > 0 && ret < sizeof(rawdata))
        {
            dst_rawdata_head_ptr->status = 0;
            msg_head_ptr->len = sizeof(MSG_HEAD_T)+sizeof(DIAG_TSX_DATA_HEAD)+ret;
            dst_rawdata_head_ptr->datalen = ret;
            memcpy((char *)dst_rawdata_head_ptr+sizeof(DIAG_TSX_DATA_HEAD), rawdata, ret);
            ((char *)(dst_rawdata_head_ptr))[sizeof(DIAG_TSX_DATA_HEAD)+ret] = 0x7e;
        }else{
            dst_rawdata_head_ptr->status = 1;
            msg_head_ptr->len = sizeof(MSG_HEAD_T)+sizeof(DIAG_TSX_DATA_HEAD);
            dst_rawdata_head_ptr->datalen = 0;
            ((char *)(dst_rawdata_head_ptr))[sizeof(DIAG_TSX_DATA_HEAD)] = 0x7e;
        }
    }else{
        ENG_LOG("%s: tsx_data cmd not read and write !!!\n", __FUNCTION__);
    }

    return msg_head_ptr->len+2;
}

static int read_tsx_data(TOOLS_DIAG_AP_TSX_DATA_T * res)
{
    int rcount;
    int ret = 0;
    FILE * fp = NULL;
    if(NULL == res)
    {
        ENG_LOG("%s: res is NULL!!!",__FUNCTION__);
        ret = -1;
        return ret;
    }
    if(access(ENG_TXDATA_FILE, F_OK) == 0) {
        ENG_LOG("%s: %s exists",__FUNCTION__, ENG_TXDATA_FILE);
        fp = fopen(ENG_TXDATA_FILE, "r");
        if(NULL == fp)
        {
            ENG_LOG("%s: fopen fail errno=%d, strerror(errno)=%s",__FUNCTION__, errno, strerror(errno));
            ret = -1;
            return ret;
        }
        rcount = fread(&res->value[0], sizeof(TSX_DATA_T), 2, fp);
        if(rcount <= 0)
        {
            ret = -1;
        }
        ENG_LOG("%s: fread count %d",__FUNCTION__, rcount);
        fclose(fp);
    }else{
        ret = -1;
        ENG_LOG("%s: %s not exists",__FUNCTION__, ENG_TXDATA_FILE);
    }
    return ret;
}

static int write_tsx_data(TOOLS_DIAG_AP_TSX_DATA_T * req)
{
    int rcount;
    int ret = 0, fd = -1;
    FILE * fp = NULL;
    mode_t old_mask;
    static first_flag = 1;
    if(NULL == req)
    {
        ENG_LOG("%s: req is NULL!!!",__FUNCTION__);
        ret = -1;
        return ret;
    }
    ENG_LOG("%s: %s exists",__FUNCTION__, ENG_TXDATA_FILE);
    if(first_flag) old_mask = umask(0);
    fp = fopen(ENG_TXDATA_FILE, "w+");
    if(first_flag) umask(old_mask);
    if(NULL == fp)
    {
        ENG_LOG("%s: fopen fail errno=%d, strerror(errno)=%s",__FUNCTION__, errno, strerror(errno));
        first_flag = 1;
        ret = -1;
        return ret;
    }
    else
    {
        first_flag = 0;
    }
    rcount = fwrite(&req->value[0], sizeof(TSX_DATA_T), 2, fp);
    ENG_LOG("%s: fread count %d",__FUNCTION__, rcount);
    if(2 != rcount)
    {
        ret = -1;
    }
    else
    {
        fflush(fp);
        fd = fileno(fp);
        if(fd > 0) {
            fsync(fd);
        } else {
            ENG_LOG("%s: fileno() error, strerror(errno)=%s", __FUNCTION__, strerror(errno));
            ret = -1;
        }
    }
    fclose(fp);
    return ret;
}

static int tsxdata_handler(char *buf, int len, char *rsp, int rsplen){
    int ret = 0;
    char *rsp_ptr;
    MSG_HEAD_T* msg_head_ptr;
    TOOLS_DIAG_AP_CNF_T* aprsp;
    TOOLS_DIAG_AP_TSX_DATA_T* src_tsxdata;
    TOOLS_DIAG_AP_TSX_DATA_T* tsxdata;
    if(NULL == buf){
        ENG_LOG("%s,null pointer",__FUNCTION__);
        return 0;
    }
    msg_head_ptr = (MSG_HEAD_T*)(buf + 1);
    rsplen = sizeof(TOOLS_DIAG_AP_TSX_DATA_T)+ sizeof(TOOLS_DIAG_AP_CNF_T) + sizeof(MSG_HEAD_T);
    rsp_ptr = (char*)malloc(rsplen);
    if(NULL == rsp_ptr){
        ENG_LOG("%s: Buffer malloc failed\n", __FUNCTION__);
        return 0;
    }
    memset(rsp_ptr, 0x00, rsplen);
    aprsp = (TOOLS_DIAG_AP_CNF_T*)(rsp_ptr + sizeof(MSG_HEAD_T));
    tsxdata = (TOOLS_DIAG_AP_TSX_DATA_T*)(rsp_ptr + sizeof(MSG_HEAD_T) + sizeof(TOOLS_DIAG_AP_CNF_T));
    memcpy(rsp_ptr,msg_head_ptr,sizeof(MSG_HEAD_T));
    ((MSG_HEAD_T*)rsp_ptr)->len = rsplen;
    aprsp->status = DIAG_AP_CMD_TSX_DATA;
    aprsp->length= sizeof(TOOLS_DIAG_AP_TSX_DATA_T);
    src_tsxdata = (TOOLS_DIAG_AP_TSX_DATA_T*)(buf + 1 + sizeof(TOOLS_DIAG_AP_CNF_T) + sizeof(MSG_HEAD_T));
    if(0 == src_tsxdata->cmd)
    {

#if 0
        /*memcpy api defect, trigger crash in androidN*/
        memcpy(tsxdata, src_tsxdata, sizeof(TOOLS_DIAG_AP_TSX_DATA_T));
        #else
        for(int y=0; y<sizeof(TOOLS_DIAG_AP_TSX_DATA_T); y++) {
            *((char *)tsxdata + y) = *((char *)src_tsxdata + y);
        }
#endif

        ret = write_tsx_data(src_tsxdata);
        if(0 == ret)
        {
            tsxdata->res_status = 0;
        }else{
            tsxdata->res_status = 1;
        }
    }else if(1 == src_tsxdata->cmd){
        ret = read_tsx_data(tsxdata);
        tsxdata->cmd = 1;
        if(0 == ret)
        {
            tsxdata->res_status = 0;
        }else{
            tsxdata->res_status = 1;
        }
    }else{
        ENG_LOG("%s: tsx_data cmd not read and write !!!\n", __FUNCTION__);
    }

out:
    rsplen = translate_packet(rsp,(unsigned char*)rsp_ptr,((MSG_HEAD_T*)rsp_ptr)->len);
    free(rsp_ptr);
    return rsplen;
}


static int read_tsx_data_ext(TOOLS_DIAG_AP_TSX_DATA_EXT_T * res)
{
    int rcount;
    int ret = 0;
    FILE * fp = NULL;
    if(NULL == res)
    {
        ENG_LOG("%s: res is NULL!!!",__FUNCTION__);
        ret = -1;
        return ret;
    }
    if(access(ENG_TXDATA_FILE, F_OK) == 0) {
        ENG_LOG("%s: %s exists",__FUNCTION__, ENG_TXDATA_FILE);
        fp = fopen(ENG_TXDATA_FILE, "r");
        if(NULL == fp)
        {
            ENG_LOG("%s: fopen fail errno=%d, strerror(errno)=%s",__FUNCTION__, errno, strerror(errno));
            ret = -1;
            return ret;
        }
        rcount = fread(&res->value[0], sizeof(TSX_DATA_EXT_T), 4, fp);
        if(rcount <= 0)
        {
            ret = -1;
        }
        ENG_LOG("%s: fread count %d",__FUNCTION__, rcount);
        fclose(fp);
    }else{
        ret = -1;
        ENG_LOG("%s: %s not exists",__FUNCTION__, ENG_TXDATA_FILE);
    }
    return ret;
}
static int write_tsx_data_ext(TOOLS_DIAG_AP_TSX_DATA_EXT_T * req)
{
    int rcount;
    int ret = 0, fd = -1;
    FILE * fp = NULL;
    mode_t old_mask;
    static first_flag = 1;
    if(NULL == req)
    {
        ENG_LOG("%s: req is NULL!!!",__FUNCTION__);
        ret = -1;
        return ret;
    }
    ENG_LOG("%s: %s exists",__FUNCTION__, ENG_TXDATA_FILE);
    if(first_flag) old_mask = umask(0);
    fp = fopen(ENG_TXDATA_FILE, "w+");
    if(first_flag) umask(old_mask);
    if(NULL == fp)
    {
        ENG_LOG("%s: fopen fail errno=%d, strerror(errno)=%s",__FUNCTION__, errno, strerror(errno));
        first_flag = 1;
        ret = -1;
        return ret;
    }
    else
    {
        first_flag = 0;
    }
    rcount = fwrite(&req->value[0], sizeof(TSX_DATA_EXT_T), 4, fp);
    ENG_LOG("%s: fread count %d",__FUNCTION__, rcount);
    if(4 != rcount)
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
        } else {
            ENG_LOG("%s: fileno() error, strerror(errno)=%s", __FUNCTION__, strerror(errno));
            ret = -1;
        }
    }
    fclose(fp);
    return ret;
}

static int tsxdata_ex_handler(char *buf, int len, char *rsp, int rsplen){
    int ret = 0;
    char *rsp_ptr;
    MSG_HEAD_T* msg_head_ptr;
    TOOLS_DIAG_AP_CNF_T* aprsp;
    TOOLS_DIAG_AP_TSX_DATA_EXT_T* src_tsxdata;
    TOOLS_DIAG_AP_TSX_DATA_EXT_T* tsxdata;
    if(NULL == buf){
        ENG_LOG("%s,null pointer",__FUNCTION__);
        return 0;
    }
    msg_head_ptr = (MSG_HEAD_T*)(buf + 1);
    rsplen = sizeof(TOOLS_DIAG_AP_TSX_DATA_EXT_T)+ sizeof(TOOLS_DIAG_AP_CNF_T) + sizeof(MSG_HEAD_T);
    rsp_ptr = (char*)malloc(rsplen);
    if(NULL == rsp_ptr){
        ENG_LOG("%s: Buffer malloc failed\n", __FUNCTION__);
        return 0;
    }
    memset(rsp_ptr, 0x00, rsplen);
    aprsp = (TOOLS_DIAG_AP_CNF_T*)(rsp_ptr + sizeof(MSG_HEAD_T));
    tsxdata = (TOOLS_DIAG_AP_TSX_DATA_EXT_T*)(rsp_ptr + sizeof(MSG_HEAD_T) + sizeof(TOOLS_DIAG_AP_CNF_T));
    memcpy(rsp_ptr,msg_head_ptr,sizeof(MSG_HEAD_T));
    ((MSG_HEAD_T*)rsp_ptr)->len = rsplen;
    aprsp->status = DIAG_AP_CMD_TSX_DATA_EXT;
    aprsp->length= sizeof(TOOLS_DIAG_AP_TSX_DATA_EXT_T);
    src_tsxdata = (TOOLS_DIAG_AP_TSX_DATA_EXT_T*)(buf + 1 + sizeof(TOOLS_DIAG_AP_CNF_T) + sizeof(MSG_HEAD_T));
    if(0 == src_tsxdata->cmd)
    {
#if 0
        /*memcpy api defect, trigger crash in androidN*/
        memcpy(tsxdata, src_tsxdata, sizeof(TOOLS_DIAG_AP_TSX_DATA_T));
        #else
        for(int y=0; y<sizeof(TOOLS_DIAG_AP_TSX_DATA_EXT_T); y++) {
        *((char *)tsxdata + y) = *((char *)src_tsxdata + y);
        }
#endif
        ret = write_tsx_data_ext(src_tsxdata);
        ENG_LOG("res_status=%d ret=%d",tsxdata->res_status, ret);
        if(0 == ret)
        {
            tsxdata->res_status = 0;
            //for bug1133893, remove tsx.dat
            if(access(ENG_TXDATA_BK_FILE, F_OK) == 0)
            {
                remove(ENG_TXDATA_BK_FILE);
            }
        }else{
            tsxdata->res_status = 1;
        }
        ENG_LOG("res_status=%d",tsxdata->res_status);
    }else if(1 == src_tsxdata->cmd){
        ret = read_tsx_data_ext(tsxdata);
        tsxdata->cmd = 1;
        if(0 == ret)
        {
            tsxdata->res_status = 0;
        }else{
            tsxdata->res_status = 1;
        }
    }else{
        ENG_LOG("%s: tsx_data cmd not read and write !!!\n", __FUNCTION__);
    }

out:
    rsplen = translate_packet(rsp,(unsigned char*)rsp_ptr,((MSG_HEAD_T*)rsp_ptr)->len);
    free(rsp_ptr);
    return rsplen;
}

void register_this_module_ext(struct eng_callback *reg, int *num)
{
    int moudles_num = 0;
    ENG_LOG("register_this_module_ext :libmiscdata");

    //1st command
    (reg+moudles_num)->type = 0x62; //main cmd 
    (reg+moudles_num)->subtype = 0x0; //sub cmd
    //reg->also_need_to_cp = 1;  //deep sleep cmd is also dealed with upon cp side
    (reg+moudles_num)->diag_ap_cmd = 0x24;
    (reg+moudles_num)->eng_diag_func = tsx_rawdata_rw_handler; // rsp function ptr
    moudles_num++;

    (reg+moudles_num)->type = 0x62; //main cmd 
    (reg+moudles_num)->subtype = 0x0; //sub cmd
    (reg+moudles_num)->diag_ap_cmd = 0x19;
    (reg+moudles_num)->eng_diag_func = tsxdata_handler; // rsp function ptr
    moudles_num++;

    (reg+moudles_num)->type = 0x62; //main cmd 
    (reg+moudles_num)->subtype = 0x0; //sub cmd
    (reg+moudles_num)->diag_ap_cmd = 0x23;
    (reg+moudles_num)->eng_diag_func = tsxdata_ex_handler; // rsp function ptr
    moudles_num++;

    *num = moudles_num;
    ENG_LOG("register_this_module_ext: %d - %d",*num, moudles_num);
}
