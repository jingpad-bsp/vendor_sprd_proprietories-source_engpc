#ifndef __ANDROID_ADAPTER_H__
#define __ANDROID_ADAPTER_H__

#ifdef __cplusplus
extern "C" {
#endif

#define CHARGER_STOP_PATH "/sys/class/power_supply/battery/stop_charge"
#define CHARGER_STOP_NEW_PATH "/sys/class/power_supply/battery/charger.0/stop_charge"

#define SYS_CLASS_ANDUSB_ENABLE "/sys/class/android_usb/android0/enable"
#define SYS_CLASS_ANDUSB_ENABLE_NEW "/config/usb_gadget/g1/UDC"

#define SYS_CLASS_ANDUSB_STATE "/sys/class/android_usb/android0/state"

#define DEVICE_SOC_USB_MAXIMUM_SPEED "sys/devices/platform/soc/soc:ap-ahb/20200000.usb/maximum_speed"
#define DEVICE_SOC_USB_MAXIMUM_SPEED_NEW "/sys/devices/platform/soc/soc:aon/5fff0000.usb/maximum_speed"


const char* OS_getUsbEnablePath();
const char* OS_getChargeStopPath();
const char* OS_getUsbMaxSpeedPath();
const char* OS_getUsbStatePath();
int OS_getUsbEnableCmd(char* buff, int nlen);

#ifdef __cplusplus
}
#endif

#endif




