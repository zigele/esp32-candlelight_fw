/*
 * @Author       : 陈科进
 * @Date         : 2023-05-18 20:38:24
 * @LastEditors  : 陈科进
 * @LastEditTime : 2023-05-30 00:11:05
 * @FilePath     : \main\usb_cherry\CherryUSB\class\usbcan\usb_can.h
 * @Description  : ESP32 candlelight firmware object
 */
#ifndef _USB_CAN_H_
#define _USB_CAN_H_
#include "usbd_core.h"
#include "usbd_help.h"
#include "usb_util.h"
#define DFU_INTERFACE_NUM 1
#define MAC_BYTES         6
char serialDescriptor[MAC_BYTES * 2 + 1];

typedef struct usbd_interface usbd_interface_t;
typedef struct usb_device_descriptor usb_device_descriptor_t;

enum GS_USB_BREQ {
    GS_USB_BREQ_HOST_FORMAT = 0,
    GS_USB_BREQ_BITTIMING,
    GS_USB_BREQ_MODE,            // 2
    GS_USB_BREQ_BERR,
    GS_USB_BREQ_BT_CONST,        // 4
    GS_USB_BREQ_DEVICE_CONFIG,
    GS_USB_BREQ_TIMESTAMP,       // 6
    GS_USB_BREQ_IDENTIFY,
    GS_USB_BREQ_GET_USER_ID,     // 8
    GS_USB_BREQ_SET_USER_ID,
    GS_USB_BREQ_DATA_BITTIMING,  // 10
    GS_USB_BREQ_BT_CONST_EXT,    // 11
    GS_USB_BREQ_SET_TERMINATION, // 12
    GS_USB_BREQ_GET_TERMINATION, // 13
    USBCAN_WINUSB_VENDOR_CODE    // 14
};
#define USBCAN_USBD_VID      0x1d50
#define USBCAN_USBD_PID      0x606f
#define USBCAN_MAX_POWER     150
#define USBCAN_LANGID_STRING 1033
#define USBCAN_BCDUSB        USB_2_1

#define USBCAN_VENDOR_ITF_NUM     0
#define USBCAN_EP_RECV            USB_ENDPOINT_OUT(2)
#define USBCAN_EP_SEND            USB_ENDPOINT_IN(1)
#define USBCAN_EP_NUMBER          0x2 //接口使用的端点数量
#define USBCAN_VENDOR_ITF_DESC    0x4 //iInterface描述这个接口的字符串描述符的索引。如果没有字符串描述符的话则此值为0。
#define SERIAL_STRING_INDEX       94  //serial 字符串在描述符数组的下标
#define VENDOR_INTERFACE_SIZE     (9 + 7 + 7)
#define USBCAN_VENDOR_CONFIG_SIZE (9 + VENDOR_INTERFACE_SIZE)

#define USBCAN_MS_OS_20_DESC_LEN 162
#define USBCAN_MAX_PACKET_SIZE   64

#endif