#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include <getopt.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include "mmiresult.h"

int eng_diag_read_mmi_interface(char *buff, int len, char *rsp, int rsplen){
    eng_init_test_file();
    return eng_diag_read_mmi(buff, len, rsp, rsplen);
}

int eng_diag_write_mmi_interface(char *buff, int len, char *rsp, int rsplen){
    return eng_diag_write_mmi(buff, len, rsp, rsplen);
}

void register_this_module_ext(struct eng_callback *reg, int *num)
{
    int i = 0;
    int moudles_num = 0;
    ENG_LOG("register_this_module_ext :libmmi_rw");

    (reg+moudles_num)->type = DIAG_CMD_APCALI_TYPE;
    (reg+moudles_num)->subtype = DIAG_CMD_APCALI_SUBTYPE;
    (reg+moudles_num)->diag_ap_cmd = DIAG_AP_CMD_READ_MMI;
    (reg+moudles_num)->eng_diag_func = eng_diag_read_mmi_interface;
    moudles_num++;

    (reg+moudles_num)->type = DIAG_CMD_APCALI_TYPE;
    (reg+moudles_num)->subtype = DIAG_CMD_APCALI_SUBTYPE;
    (reg+moudles_num)->diag_ap_cmd = DIAG_AP_CMD_WRITE_MMI;
    (reg+moudles_num)->eng_diag_func = eng_diag_write_mmi_interface;
    moudles_num++;

    *num = moudles_num;
    ENG_LOG("register_this_module_ext: %d - %d",*num, moudles_num);
}
