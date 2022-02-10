#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <cutils/log.h>
#include <cutils/properties.h>
#include <cutils/sockets.h>

#include "engpcctl.h"

void usage(){
    printf("Usage:\n");
    printf("  engpcctl dst act [param1] [param2] [...]\n");
    printf("  such as: engpcctl chnl enable normal PC-CP-PPPD\n");
    printf("  dst: \n");
    printf("       dev, chnl\n");
    printf("  act: \n");
    printf("       list\n");
    printf("       add\n");
    printf("       del\n");
    printf("       enable\n");
    printf("       disable\n");
}

int getDst(char* buff){
    if (buff == NULL){
        return -1;
    }

    if (strcasecmp(buff, "dev") == 0){
        return ENGPC_DST_DEV;
    }

    if (strcasecmp(buff, "chnl") == 0){
        return ENGPC_DST_CHNL;
    }

    return -1;
}

int getAct(char* buff){
    if (buff == NULL){
        return -1;
    }

    if(strcasecmp(buff, "list") == 0){
        return ENGPC_ACT_LIST;
    }
    if(strcasecmp(buff, "add") == 0){
        return ENGPC_ACT_ADD;
    }
    if(strcasecmp(buff, "del") == 0){
        return ENGPC_ACT_DEL;
    }
    if(strcasecmp(buff, "enable") == 0){
        return ENGPC_ACT_ENABLE;
    }
    if(strcasecmp(buff, "disable") == 0){
        return ENGPC_ACT_DISABLE;
    }

    return -1;
}

int main(int argc, char *argv[]) {
    char* param[16] = {NULL};

    if (argc < 2){
        usage();
        return 0;
    }

    for(int i = 0; i < argc; i++){
        param[i] = argv[i];
    }

    int dst = getDst(argv[1]);
    int act = getAct(argv[2]);
    if (dst == -1 || act == -1){
        printf("dst = %d, act = %d", dst, act);
        usage();
        return 0;
    }

    if (dst == ENGPC_DST_DEV){
        engpc_ctl_dev((ENGPC_ACT)act, param[3], param[4], param[5], param[6], param[7], param[8]);
    }else if(dst == ENGPC_DST_CHNL){
        engpc_ctl_chnl((ENGPC_ACT)act, param[3], param[4], param[5], param[6], param[7], param[8], param[9], param[10], param[11], param[12]);
    }else{
    }

    return 0;
}