/*
 * Copyright (C) 2012 Spreadtrum Communications Inc.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include <getopt.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <utils/Log.h>
#include <cutils/properties.h>
#include "sprd_fts_type.h"
#include "sprd_fts_log.h"
#include "eng_attok_test.h"

#define AT_PROP_OP "AT+PROP"

#define AT_GET_PROP_FLAG "at+getprop"
#define AT_SET_PROP_FLAG "at+setprop"

static int get_prop(char *req, char *rsp);
static int set_prop(char *req, char *rsp);
static int get_prop_intenal(char *prop, char *buf);

static int get_prop_intenal(char *prop, char *buf)
{
    static char desc[256];
    int ret = -1;
    char relver[PROPERTY_VALUE_MAX];
    ALOGD("get_prop_intenal prop:%s,strlen=%d", prop,strlen(prop));
    ret = property_get(prop, relver, NULL);
    if(ret <= 0) {
        return -1;
    }
    //snprintf(desc, 128, "AutoTest for android %s, %s %s", relver, __DATE__, __TIME__ );
    ALOGD("get_prop_intenal:%s", relver);
    ALOGD("get_prop_intenal sizeof(buf):%d", sizeof(buf));
    //snprintf(buf, sizeof(buf), "%s", relver);
    sprintf(buf, "%s", relver);
    return ret;
}

static int get_prop(char *req, char *rsp)
{
    int ret;
    char *ptr = NULL;
    char *in_cmd_buf = NULL;
    bool is_diag_cmd = false;

    if (NULL == req)
    {
        ALOGE("%s,null pointer", __FUNCTION__);
        sprintf(rsp, "\r\nERROR\r\n");
        return rsp != NULL ? strlen(rsp) : 0;
    }
    ALOGD("get_prop req:%s", req);
    if(req[0] == 0x7e)
    {
        ptr = req + 1 + sizeof(MSG_HEAD_T);
        is_diag_cmd = true;
        ALOGD("get_prop strlen(ptr)=%d", strlen(ptr));
    }
    else
    {
        ptr = strdup(req) ;
    }
    ALOGD("get_prop:%s \nis_diag_cmd=%d", ptr,is_diag_cmd);
    
    ret = at_tok_start_test(&ptr);
    ALOGD("get_prop at_tok_start ret:%d", ret);
    ret |= at_tok_nextstr(&ptr, &in_cmd_buf);
    if (NULL == in_cmd_buf)
    {
        ALOGE("%s,in_cmd_buf null pointer", __FUNCTION__);
        sprintf(rsp, "\r\nERROR\r\n");
        return rsp != NULL ? strlen(rsp) : 0;
    }

    strtok(in_cmd_buf, "~");
    ALOGD("get_prop in_cmd_buf:%s", in_cmd_buf);
    strtok(in_cmd_buf, "\r");
    ALOGD("get_prop in_cmd_buf22:%s", in_cmd_buf);
    strtok(in_cmd_buf, "\n");
    ALOGD("get_prop in_cmd_buf23:%s", in_cmd_buf);
    ret = get_prop_intenal(in_cmd_buf,rsp);
    ALOGD("get_prop_intenal ret:%d", ret);
    ALOGD("get_prop_intenal rsp:%s", rsp);
    if((false == is_diag_cmd) && (NULL != ptr))
    {
        free(ptr);
    }

    if(ret <= 0)
    {
        sprintf(rsp, "\r\nERROR\r\n");
    }else{
        //sprintf(rsp, "\r\nOK\r\n");
    }

    return rsp != NULL ? strlen(rsp) : 0;
}
static int set_prop(char *req, char *rsp)
{
    int ret;
    return ret;
}

#define ENG_STREND "\r\n"

static int prop_handler(char *req, char *rsp){
    int ret;
    char *ptr = NULL;
    char *in_cmd_buf = NULL;
    bool is_diag_cmd = false;

    if (NULL == req)
    {
        ALOGE("%s,null pointer", __FUNCTION__);
        sprintf(rsp, "\r\nERROR\r\n");
        return rsp != NULL ? strlen(rsp) : 0;
    }
    ALOGD("get_prop req:%s", req);
    if(req[0] == 0x7e)
    {
        ptr = req + 1 + sizeof(MSG_HEAD_T);
        is_diag_cmd = true;
        ALOGD("get_prop strlen(ptr)=%d", strlen(ptr));
    }
    else
    {
        ptr = strdup(req) ;
    }
    ALOGD("get_prop:%s \nis_diag_cmd=%d", ptr,is_diag_cmd);

    char ptr_cmd[1];
    char *ptr_key, *ptr_val;
    char buf[PROPERTY_VALUE_MAX] = {0};

    req = strchr(ptr, '=');
    req++;
    ptr_cmd[0] = *req;
    req = strchr(req, ',');
    req++;
    if (*req != '[') {
        return -1;
    }
    req++;
    ptr_key = req;
    req = strchr(req, ']');
    *req = '\0';

    if (ptr_cmd[0] == '0') {
        property_get(ptr_key, buf, "NOT FOUND");
        if (!strncmp(buf, "NOT FOUND", sizeof("NOT FOUND") - 1)) {
            sprintf(rsp, "%s%s%s", ENG_STREND, buf, ENG_STREND);
        } else {
            sprintf(rsp, "%s%s%s%s%s", ENG_STREND, "[", buf, "]", ENG_STREND);
        }
    } else if (ptr_cmd[0] == '1') {
        req++;
        if (*req != '[') {
            return -1;
        }
        req++;
        ptr_val = req;
        req = strchr(req, ']');
        *req = '\0';
        property_set(ptr_key, ptr_val);
        sprintf(rsp, "%s%s%s%s%s%s%s", ENG_STREND, "[", ptr_key, "][", ptr_val, "]",
        ENG_STREND);
    }
    return 0;
}

extern "C" {
void register_this_module_ext(struct eng_callback *reg, int *num)
{
    unsigned int moudles_num = 0;

    sprintf((reg+moudles_num)->at_cmd, "%s", AT_GET_PROP_FLAG);
    (reg+moudles_num)->eng_linuxcmd_func = get_prop;
    ALOGD("module cmd:%s\n", (reg+moudles_num)->at_cmd);
    moudles_num ++;

    sprintf((reg+moudles_num)->at_cmd, "%s", AT_PROP_OP);
    (reg+moudles_num)->eng_linuxcmd_func = prop_handler;
    ALOGD("module cmd:%s\n", (reg+moudles_num)->at_cmd);
    moudles_num ++;


    *num = moudles_num;
}
}
