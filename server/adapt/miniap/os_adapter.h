#ifndef __ANDROID_ADAPTER_H__
#define __ANDROID_ADAPTER_H__

#ifdef __cplusplus
extern "C" {
#endif

#define USB_CONFIG_VSER "vser"

#ifdef ENGPC_VSER_INIT
#define PROP_USB_CONFIG             "vendor.flag.sys.usb.config"
#define PROP_USB_CONFIG_VALUE       "1"
#define PROP_USB_STATE              "vendor.sys.usb.state"
#else
#define PROP_USB_CONFIG             "sys.usb.config"
#define PROP_USB_STATE              "sys.usb.state"
#define PROP_USB_CONFIG_VALUE       USB_CONFIG_VSER
#endif

#define SYSTEM_PROP_BOOTMODE "ro.bootmode"
#define VENDOR_PROP_BOOTMODE "vendor.ro.bootmode"

#define CHARGER_STOP_PATH "/sys/class/power_supply/battery/stop_charge"
#define CHARGER_STOP_NEW_PATH "/sys/class/power_supply/battery/charger.0/stop_charge"

#define SYS_CLASS_ANDUSB_ENABLE "/sys/class/android_usb/android0/enable"
#define SYS_CLASS_ANDUSB_ENABLE_NEW "/config/usb_gadget/g1/UDC"

#define SYS_CLASS_ANDUSB_STATE "/sys/class/android_usb/android0/state"

#define DEVICE_SOC_USB_MAXIMUM_SPEED "sys/devices/platform/soc/soc:ap-ahb/20200000.usb/maximum_speed"
#define DEVICE_SOC_USB_MAXIMUM_SPEED_NEW "/sys/devices/platform/soc/soc:aon/5fff0000.usb/maximum_speed"

void OS_usb_vser_enable(int mode);
const char* OS_usb_getEnablePath();
const char* OS_usb_getMaxSpeedPath();
const char* OS_usb_getStatePath();
int OS_usb_getConf(char* buff, int nlen);

const char* OS_getChargeStopPath();

int OS_is_data_dir_ready();

int wait_for_usbenum_succ(int timeout, char *state);

#define PROP_MODEM_LOG_DEST    "persist.vendor.modem.log_dest"
#define PROP_WCN_LOG_DEST      "persist.vendor.wcn.log_dest"

void OS_sys_getlogdest(char* log_type, char* location);
void OS_sys_setlogdest(char* log_type, char* location);
void OS_sys_restart_self();
int OS_sys_gettid();

#ifdef __cplusplus
}
#endif

#endif




