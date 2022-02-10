#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "sprd_fts_type.h"
#include "sprd_fts_log.h"
#include "sprd_fts_diag.h"

static int swpac_version_get(char *req, char *rsp)
{
    int fd = 0;
    ssize_t sizes;
    char rsptemp[2048] = {0};
    if (NULL == req){
        ENG_LOG("%s: null pointer, err: %s\n", __func__, strerror(errno));
        strcpy(rsp, "\r\nERROR\r\n");
        return errno;
    }

    fd = open("/vendor/etc/SWPACInfo.txt", O_RDONLY);
    if (fd < 0) {
        strcpy(rsp, "\r\nERROR\r\n");
        ENG_LOG("%s: open file failed, err: %s\n", __func__, strerror(errno));
        return errno;
    }

    ENG_LOG("libgetswpac :enter function test :AT+GETSWPAC?");
    sizes = read(fd, rsptemp, sizeof(rsptemp) - 1);
    if (sizes < 0) {
        strcpy(rsp, "\r\nERROR\r\n");
        ENG_LOG("%s: read file failed, err: %s\n", __func__, strerror(errno));
        close(fd);
        return errno;
    }
    snprintf(rsp,2048,"+GETSWPAC:%s", rsptemp);
    ENG_LOG("AT+GETSWPAC? return %s",rsp);
    close(fd);

    return strlen(rsp);
}


void register_this_module_ext(struct eng_callback *reg, int *num)
{
    int moudles_num = 0;
    ENG_LOG("register_this_module_ext :libgetswpac");

    sprintf(reg->at_cmd, "%s", "AT+GETSWPAC?");
    reg->eng_linuxcmd_func = swpac_version_get; // rsp function ptr
    moudles_num++;

    *num = moudles_num;
    ENG_LOG("register_this_module_ext: %d - %d",*num, moudles_num);
}