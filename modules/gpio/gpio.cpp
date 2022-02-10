#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include <getopt.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <dirent.h>
#include "sprd_fts_type.h"
#include "sprd_fts_log.h"
#include "pinconfig.h"

#include <cutils/properties.h>
#include <linux/version.h>

#ifndef KERNEL_OLD
#include <linux/autotest.h>
#endif

//------------------------------------------------------------------------------
#define DEV_PIN_CONFIG_PATH1     "/sys/kernel/debug/pinctrl/e42a0000.pinctrl/pins_debug"
#define DEV_PIN_CONFIG_PATH2    "/sys/kernel/debug/pinctrl/402a0000.pinctrl/pins_debug"
#define AUTOTEST_DEV_FILE	"/dev/autotest0"

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//====================================================================
//add the setgpio function set enable or disable gpio
//step one : pin offset exchange the gpio function
//step two : check the gpio struct path
//step three : exchange the gpio function to export mode
//step four: operation the gpio function disable or enable
//step five : get the reslut the operation of the gpio
//====================================================================

int Setgpio_value(int gpio_num,char value)
{
	int ret = -1;
	char gpio_out[255] = {0};
	char out_cmd[255] = {0};
	char setval_cmd[255] = {0};
	char gpio_setval_path[255] = {0};
	int fd= -1;

	ENG_LOG("%s enter!gpio%d value:%d\n", __FUNCTION__, gpio_num, value );

	//moidify the gpionum list set out direction function
	sprintf(gpio_out, "/sys/class/gpio/gpio%d/direction", gpio_num);
	if(0 != access(gpio_out, F_OK)){
		LOGD("the path can`t find %s\n", gpio_out);
		return  ret;
	}
	sprintf(out_cmd, "echo out > %s", gpio_out);
	system(out_cmd);
	usleep(100);

	//moidify the gpionum list set out direction function
	sprintf(gpio_setval_path, "/sys/class/gpio/gpio%d/value", gpio_num);
	if(0 != access(gpio_setval_path, F_OK)){
		ENG_LOG("the path can`t find %s\n", gpio_setval_path);
		return  ret;
	}
	sprintf(setval_cmd, "echo %d > %s", value, gpio_setval_path);
	system(setval_cmd);
	usleep(100);

	char gpio_enable_status[3] = {0};
	if (0 == access(gpio_setval_path , F_OK)){
		fd = open(gpio_setval_path, O_RDONLY);
		if(fd < 0){
			ENG_LOG("the %s--- open fail file %s\n", __FUNCTION__, gpio_setval_path);
			return ret;
		}else{
			memset(gpio_enable_status, 0, sizeof(gpio_enable_status));
			ret = read(fd , gpio_enable_status, 3);
			if(ret < 0){
				ENG_LOG("the %s--- read fail file %s\n", __FUNCTION__, gpio_setval_path);
				close(fd);
				return ret;
			}
			gpio_enable_status[2] = '\0';
			ret = atoi(gpio_enable_status);
			ENG_LOG("%s exit! gpio%d value:%d \n",__FUNCTION__, gpio_num, ret);
			close(fd);
		}
	}else{
		ENG_LOG("the %s---filepath can`t find  %s\n", __FUNCTION__, gpio_setval_path);
	}
	usleep(200);

	return 0;
}

int Getgpio_value(int gpio_num, char *value)
{
	char gpio_enable_status[3] = {0};
	char gpio_setval_path[255] = {0};
	int fd= -1, ret;
	char gpio_in[255] = {0};
	char in_cmd[255] = {0};

	//moidify the gpionum list set in direction function
	sprintf(gpio_in, "/sys/class/gpio/gpio%d/direction", gpio_num);
	if(0 != access(gpio_in, F_OK)){
		ENG_LOG("the path can`t find %s\n", gpio_in);
		return  -1;
	}
	sprintf(in_cmd, "echo in > %s", gpio_in);
	system(in_cmd);
	usleep(100);

	ENG_LOG("%s enter!get gpio%d value\n", __FUNCTION__, gpio_num);
	sprintf(gpio_setval_path, "/sys/class/gpio/gpio%d/value", gpio_num);
	if(0 != access(gpio_setval_path, F_OK)){
		ENG_LOG("the path can`t find %s\n", gpio_setval_path);
		return  -1;
	}

	fd = open(gpio_setval_path, O_RDONLY);
	if(fd < 0){
		ENG_LOG("the %s--- open fail file %s\n", __FUNCTION__, gpio_setval_path);
		return -1;
	}else{
		memset(gpio_enable_status, 0, sizeof(gpio_enable_status));
		ret = read(fd , gpio_enable_status, 3);
		if(ret < 0){
			LOGD("the %s--- read fail file %s\n", __FUNCTION__, gpio_setval_path);
			close(fd);
			return -1;
		}
		gpio_enable_status[2] = '\0';
		*value = (char)atoi(gpio_enable_status);
		close(fd);
		ENG_LOG("%s exit! gpio%d value:%d \n", __FUNCTION__, gpio_num, *value);
	}

	return 1;
}

//----------------------------------------------------------------------------
void  cam_sd_tcard_addvoltage(void)
{
#ifdef KERNEL_OLD
	system("echo 1 > /sys/kernel/debug/sprd-regulator/vddcamio/enable");
	usleep(50);
	system("echo 1 > /sys/kernel/debug/sprd-regulator/vddsdcore/enable");
	usleep(50);
	system("echo 1 > /sys/kernel/debug/sprd-regulator/vddsdio/enable");
	usleep(50);
	system("echo 1 > /sys/kernel/debug/sprd-regulator/vddsd/enable");
	usleep(50);
	system("echo 1 > /sys/kernel/debug/sprd-regulator/vddsim0/enable");
	usleep(50);
	system("echo 1 > /sys/kernel/debug/sprd-regulator/vddsim1/enable");
	usleep(50);
#endif
}

static int get_ap_gpio_base(void)
{
	int fd, ret;
	DIR *dir;
	struct dirent *de;
	char filename[256] = {0};
	char buf[128] = {0};

	dir = opendir("/sys/class/gpio");
	if (!dir) {
		ENG_LOG("open /sys/class/gpio failed, errno=%d(%s)\n", errno, strerror(errno));
		return -1;
	}

	while ((de = readdir(dir))) {
		if (strncmp(de->d_name, "gpiochip", strlen("gpiochip")))
			continue;
		memset(filename, 0, sizeof(filename));
		sprintf(filename, "/sys/class/gpio/%s/%s", de->d_name, "ngpio");
		fd = open(filename, O_RDONLY);
		if (fd < 0) {
			ENG_LOG("open %s failed, errno=%d(%s)\n", filename, errno, strerror(errno));
			continue;
		}
		memset(buf, 0, sizeof(buf));
		read(fd, buf, sizeof(buf) - 1);
		close(fd);
		fd = -1;
		ret = atoi(buf);
		if (ret == 256) {
			memset(filename, 0, sizeof(filename));
			sprintf(filename, "/sys/class/gpio/%s/%s", de->d_name, "base");
			fd = open(filename, O_RDONLY);
			if (fd < 0) {
				ENG_LOG("open %s failed, errno=%d(%s)\n", filename, errno, strerror(errno));
				continue;
			}
			memset(buf, 0, sizeof(buf));
			read(fd, buf, sizeof(buf) - 1);
			close(fd);
			fd = -1;
			ret = atoi(buf);
			closedir(dir);
			dir = NULL;
			return ret;
		}
	}

	closedir(dir);
	dir = NULL;
	return -1;
}

/*
GPIO PC-->engpc:	7E 4C 07 00 00 0D 00 38 07 01 xx 00 xx xx 7E	GPO output "0"	" GPIO输出0或1 红色字段xx代表gpio号码 ,蓝色字段xx xx 代表软件偏移地址"
	 engpc-->pc:    7E 00 00 00 00 08 00 38 00 7E
	 PC-->engpc:    7E 4C 07 00 00 0D 00 38 07 01 xx 01 xx xx 7E	GPO output "1"
	 engpc-->pc:    7E 00 00 00 00 08 00 38 00 7E
	 PC-->engpc:    7E 4C 07 00 00 0D 00 38 07 00 xx yy xx xx 7E	GPI in	"红色字段xx代表gpio号码 ,蓝色字段xx xx 代表软件偏移地址，命令返回GPI读取到的状态         yy 0：输入下拉 1：输入上拉"
	 engpc-->pc:    7E 00 00 00 00 09 00 38 00  xx 7E 		xx代表读到的GPIO值，为01或00。
**/
int testGpio(char *buf, int len, char *rsp, int rsplen)
{
	int ret = 0;  //用于表示具体测试函数返回值，
	/****************************************************************
	ret <0,表示函数返回失败。
	ret =0,表示测试OK，无需返回数据。
	ret >0,表示测试OK，获取ret个数据，此时，ret的大小表示data中保存的数据的个数。
	***********************************************************************/
	int i, base, fd, gpio_data = 0;
	char cmd[255] = {0};
	char export_cmd[255] = {0};
	char pin_addr[15] = {0};
	int  dir = buf[9];
	int  gpio_num = buf[10];
	char  value = buf[11];
	int maxnum = sizeof(gpionumber)/sizeof(struct GPIO_ADDRESS);

	FUN_ENTER; //打印函数提名字：testGpio ++

	//print TARGET_BOARD_PLATFORM to verify soc platform
	property_get("ro.board.platform", cmd, "0");
	ENG_LOG("%s: TARGET_BOARD_PLATFORM = %s\n", __FUNCTION__, cmd);
	memset(cmd, 0, sizeof(cmd));

	//打印PC下发的数据。
	ENG_LOG("pc->engpc:%d number--> ",len); //78 xx xx xx xx _ _38 _ _ 打印返回的10个数据
	for (i = 0; i < len; i++)
		LOGD("%x ", buf[i]);

	if (gpio_num >= maxnum) {
		ENG_LOG("gpio_num:%d exceed maxnum:%d", gpio_num, maxnum);
		ret = -1;
		goto exit;
	}

	//如下为功能代码
	cam_sd_tcard_addvoltage();
	sprintf(pin_addr, "0x%x", gpionumber[gpio_num].bias_address);
	ENG_LOG("testGpio  value  pin_addr = %s!!!gpio_num= %d, maxnum=%d\n",
			pin_addr,gpio_num, maxnum);

#ifdef KERNEL_OLD
	ENG_LOG("kernel version is smaller than 4.14\n");

	if(0 == access(DEV_PIN_CONFIG_PATH1, F_OK)) {
		sprintf(cmd, "echo %s > %s", pin_addr,DEV_PIN_CONFIG_PATH1);
	} else if(0 == access(DEV_PIN_CONFIG_PATH2, F_OK)) {
		sprintf(cmd, "echo %s > %s", pin_addr,DEV_PIN_CONFIG_PATH2);
	} else {
		ENG_LOG("cann't find the  e42a0000 and  402a0000 path\n");
		ret = -1;
		goto exit;
	}
	system(cmd);
	ENG_LOG("the pad to gpio function = %s\n", cmd);
	usleep(500);

	//add the gpio_num list path
	if(0 != access("/sys/class/gpio/export", F_OK)){
		ENG_LOG("the path can`t find /sys/class/gpio/export node!\n");
		ret = -1;
		goto exit;
	}
	sprintf(export_cmd, "echo %d > /sys/class/gpio/export", gpio_num);
	//modify the gpio function set export function
	system(export_cmd);
#else
	ENG_LOG("kernel version is 4.14 or larger than 4.14\n");

	fd = open(AUTOTEST_DEV_FILE, O_RDWR);
	if (fd < 0) {
		ENG_LOG("open /dev/autotest0 node failed.\n");
		goto exit;
	}

	ret = ioctl(fd, AT_PINCTRL, &gpio_num);
	if (ret < 0) {
		ENG_LOG("ioctl /dev/autotest0 node failed %d.\n", ret);
		close(fd);
		goto exit;
	}
	usleep(500);

	base = get_ap_gpio_base();
	if (base < 0) {
		ret = -1;
		close(fd);
		goto exit;
	}
#endif
	usleep(200);

	switch(dir)
	{
		case 1:
#ifdef KERNEL_OLD
		   ret = Setgpio_value(gpio_num, value);
#else
		   gpio_data = ((base + gpio_num) & 0xffff) | (1 << 16) |
			   ((!!value) << 17); //num,direction,value
		   ret = ioctl(fd, AT_GPIO, &gpio_data);
#endif
		   break;
		case 0:
#ifdef KERNEL_OLD
		   ret = Getgpio_value(gpio_num, &value);
#else
		   gpio_data = ((base + gpio_num) & 0xffff);
		   ret = ioctl(fd, AT_GPIO, &gpio_data);
		   if (!ret) {
			value = !!(gpio_data & (1 << 17)); //bit17 from driver
			ret = 1;
		   }
#endif
		  break;
		default:
		    break;
	}
	close(fd);
	ENG_LOG("ret = %d!!!\n  ", ret);

exit:
	/*------------------------------后续代码为通用代码，所有模块可以直接复制，------------------------------------------*/

	//填充协议头7E xx xx xx xx 08 00 38
	MSG_HEAD_T *msg_head_ptr;
	memcpy(rsp, buf, 1 + sizeof(MSG_HEAD_T)-1);  //复制7E xx xx xx xx 08 00 38至rsp,，减1是因为返回值中不包含MSG_HEAD_T的subtype（38后面的数据即为subtype）
	msg_head_ptr = (MSG_HEAD_T *)(rsp + 1);

	//修改返回数据的长度，如上复制的数据长度等于PC发送给engpc的数据长度，现在需要改成engpc返回给pc的数据长度。
	msg_head_ptr->len = 8; //返回的最基本数据格式：7E xx xx xx xx 08 00 38 xx 7E，去掉头和尾的7E，共8个数据。如果需要返回数据,则直接在38 XX后面补充


	//填充成功或失败标志位rsp[1 + sizeof(MSG_HEAD_T)]，如果ret>0,则继续填充返回的数据。
	ENG_LOG("msg_head_ptr,ret=%d",ret);

	if(ret<0)
	{
		rsp[sizeof(MSG_HEAD_T)] = 1;    //38 01 表示测试失败。
	}else if (ret==0){
		rsp[sizeof(MSG_HEAD_T)] = 0;    //38 00 表示测试ok
	}else if(ret >0){
		rsp[sizeof(MSG_HEAD_T)] = 0;    //38 00 表示测试ok,ret >0,表示需要返回 ret个字节数据。
		memcpy(rsp + 1 + sizeof(MSG_HEAD_T), &value, ret);	  //将获取到的ret个数据，复制到38 00 后面
		msg_head_ptr->len+=ret;   //返回长度：基本数据长度8+获取到的ret个字节数据长度。
	}
	ENG_LOG("rsp[1 + sizeof(MSG_HEAD_T):%d]:%d",sizeof(MSG_HEAD_T),rsp[sizeof(MSG_HEAD_T)]);
	//填充协议尾部的7E
	rsp[msg_head_ptr->len + 2 - 1]=0x7E;  //加上数据尾标志
	ENG_LOG("dylib test :return len:%d",msg_head_ptr->len + 2);
	ENG_LOG("engpc->pc:%x %x %x %x %x %x %x %x %x %x",rsp[0],rsp[1],rsp[2],rsp[3],rsp[4],rsp[5],rsp[6],rsp[7],rsp[8],rsp[9]); //78 xx xx xx xx _ _38 _ _ 打印返回的10个数据
	FUN_EXIT; //打印函数体名字
	return msg_head_ptr->len + 2;
	/*------------------------------如上虚线之间代码为通用代码，直接赋值即可---------*/
}

extern "C" void register_this_module_ext(struct eng_callback *reg, int *num)
{
	int moudles_num = 0;   //用于表示注册的节点的数目。
	ENG_LOG("register_this_module :gpio\n");

//1st command
   (reg + moudles_num)->type = 0x38;  //38对应BBAT测试
   (reg + moudles_num)->subtype = 0x07;   //0x07对应BBAT中的GPIO测试。
   (reg + moudles_num)->diag_ap_cmd  = 0x00; //set gpio value
   (reg + moudles_num)->eng_diag_func = testGpio; //testGpio对应BBAT中GPIO测试功能函数，见上。
   moudles_num++;

   (reg + moudles_num)->type = 0x38;  //38对应BBAT测试
   (reg + moudles_num)->subtype = 0x07;   //0x07对应BBAT中的GPIO测试。
   (reg + moudles_num)->diag_ap_cmd  = 0x01; //get gpio value
   (reg + moudles_num)->eng_diag_func = testGpio; //testGpio对应BBAT中GPIO测试功能函数，见上。
   moudles_num++;

//1st command
   (reg + moudles_num)->type = 0x38;  //38对应BBAT测试
   (reg + moudles_num)->subtype = 0x07;   //0x07对应BBAT中的GPIO测试。
   (reg + moudles_num)->diag_ap_cmd  = 0x03; //set gpio value
   (reg + moudles_num)->also_need_to_cp = true; //03 转至CP侧处理。
   moudles_num++;

   (reg + moudles_num)->type = 0x38;  //38对应BBAT测试
   (reg + moudles_num)->subtype = 0x07;   //0x07对应BBAT中的GPIO测试。
   (reg + moudles_num)->diag_ap_cmd  = 0x02; //get gpio value
   (reg + moudles_num)->also_need_to_cp = true; //02 转至CP侧处理。
   moudles_num++;

#if 0
	//1st command
	reg->type = 0x38;  //38对应BBAT测试
	reg->subtype = 0x07;   //0x07对应BBAT中的GPIO测试。
	reg->eng_diag_func = testGpio; //testGpio对应BBAT中GPIO测试功能函数，见上。
	moudles_num++;

	/*************
	//2st command
	reg->type = 0x39;  //39对应nativemmi测试
	reg->subtype = 0x07;   //0x07对应nativemmi中的GPIO测试。
	reg->eng_diag_func = testnativeGpio;
	moudles_num++;
	**********************/
#endif
	*num = moudles_num;
	ENG_LOG("register_this_module_ext: %d - %d",*num, moudles_num);
}
