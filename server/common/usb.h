#ifndef __ENGPC_USB_H__
#define __ENGPC_USB_H__

typedef enum {
  USB_SPEED_UNKNOWN = 0,
  /* enumerating */
  USB_SPEED_LOW, USB_SPEED_FULL,
  /* usb 1.1 */
  USB_SPEED_HIGH,
  /* usb 2.0 */
  USB_SPEED_WIRELESS,
  /* wireless (usb 2.5) */
  USB_SPEED_SUPER,
  /* usb 3.0 */
}USB_DEVICE_SPEED_ENUM;

struct uevent {
  const char *action;
  const char *path;
  const char *subsystem;
  const char *usb_connect;
};

#define UEVENT_MSG_LEN 1024

typedef enum{
    USB_CONNECT,
    USB_DISCONNECT,

    USB_UNKNOWN,
}UEVENT_MSG;

typedef int (*uevent_notify)(UEVENT_MSG msg, void* param);

int disconnect_vbus_charger(void);
int connect_vbus_charger(void);
int eng_usb_state(void);
void eng_usb_maximum_speed(USB_DEVICE_SPEED_ENUM speed);
int eng_usb_config(char* buff, int nlen);
int usb_mode(const char* bootmode);

void parse_event(const char *msg, struct uevent *uevent);
void handle_device_event(struct uevent *uevent);
void handle_device_fd(int sock);
void* eng_uevt_thread(void* x);
void usb_monitor(uevent_notify ptrNotify);

#endif