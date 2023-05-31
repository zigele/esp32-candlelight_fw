/*
 * @Author       : 陈科进
 * @Date         : 2023-05-18 20:38:24
 * @LastEditors  : 陈科进
 * @LastEditTime : 2023-05-21 18:01:29
 * @FilePath     : \main\usb_cherry\CherryUSB\class\usbcan\usbd_can.h
 * @Description  : ESP32 candlelight firmware object
 */
#ifndef _USBD_CAN_H_
#define _USBD_CAN_H_

#include "usb_can.h"
#include "can.h"

extern void usbcan_out_callback(uint8_t ep, uint32_t nbytes);
extern void usncan_in_callback(uint8_t ep, uint32_t nbytes);

#ifdef __cplusplus
extern "C" {
#endif
struct usbd_interface *usbd_usbcan_init_intf(usbd_interface_t *intf);
void winusb_init(void);
void usbcan_sreial_string_register();
#ifdef __cplusplus
}
#endif

#endif /* _USBD_XXX_H_ */
