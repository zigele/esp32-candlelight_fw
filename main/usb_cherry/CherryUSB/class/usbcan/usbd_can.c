/*
 * @Author       : 陈科进
 * @Date         : 2023-05-18 20:38:24
 * @LastEditors  : 陈科进
 * @LastEditTime : 2023-05-29 22:24:03
 * @FilePath     : \main\usb_cherry\CherryUSB\class\usbcan\usbd_can.c
 * @Description  : ESP32 candlelight firmware object
 */
#include "usbd_can.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "gs_usb.h"
#include "others/term_resistor.h"
#include "others/led.h"
typedef struct
{
    twai_state_t state;
} can_channel_t;
can_channel_t *hCAN;
can_channel_t *channels[NUM_CAN_CHANNEL] = { &hCAN };
char serialDescriptor[MAC_BYTES * 2 + 1] = { '\0' };
static uint64_t sof_timestamp_us = 0;
bool timestamps_enabled;
struct usbd_interface intf0;

static const gs_device_config_t USBD_GS_CAN_dconf = {
    .reserved1 = 0,
    .reserved2 = 0,
    .reserved3 = 0,
    .icount = 0,
    .sw_version = 2,
    .hw_version = 0
};

static const gs_device_bt_const_t USBD_GS_CAN_btconst = {
    .feature = GS_CAN_FEATURE_LISTEN_ONLY | GS_CAN_FEATURE_LOOP_BACK | GS_CAN_FEATURE_HW_TIMESTAMP | GS_CAN_FEATURE_IDENTIFY | GS_CAN_FEATURE_PAD_PKTS_TO_MAX_PKT_SIZE | GS_CAN_FEATURE_TERMINATION,
    .fclk_can = CAN_CLOCK_SPEED,
    .tseg1_min = 1,
    .tseg1_max = 20,
    .tseg2_min = 1,
    .tseg2_max = 16,
    .sjw_max = 8,
    .brp_min = 1,
    .brp_max = 1024,
    .brp_inc = 1
};

uint8_t usbcan_descriptor[] = {
    USB_DEVICE_DESCRIPTOR_INIT(USBCAN_BCDUSB, USB_DEVICE_CLASS_VEND_SPECIFIC, 0x00, 0x00, USBCAN_USBD_VID, USBCAN_USBD_PID, 0x0100, 0x01),
    /* Configuration 0 */
    USB_CONFIG_DESCRIPTOR_INIT(USBCAN_VENDOR_CONFIG_SIZE, 0x01, 0x01, USB_CONFIG_BUS_POWERED, USBCAN_MAX_POWER),
    /* Interface 0 */
    USB_INTERFACE_DESCRIPTOR_INIT(USBCAN_VENDOR_ITF_NUM, 0x00, USBCAN_EP_NUMBER, 0xFF, 0xFF, 0x00, USBCAN_VENDOR_ITF_DESC),
    /* Endpoint OUT 1 */
    USB_ENDPOINT_DESCRIPTOR_INIT(USBCAN_EP_RECV, USB_ENDPOINT_TYPE_BULK, USBCAN_MAX_PACKET_SIZE, 0x01),
    /* Endpoint IN 2 */
    USB_ENDPOINT_DESCRIPTOR_INIT(USBCAN_EP_SEND, USB_ENDPOINT_TYPE_BULK, USBCAN_MAX_PACKET_SIZE, 0x01),
    /* String 0 (LANGID) */
    USB_LANGID_INIT(USBCAN_LANGID_STRING),

    /* String 1 (Manufacturer) */
    0x0E,                       // bLength
    USB_DESCRIPTOR_TYPE_STRING, // bDescriptorType
    'e', 0,                     // wcChar0
    'a', 0,                     // wcChar1
    's', 0,                     // wcChar2
    'y', 0,                     // wcChar3
    '3', 0,                     // wcChar4
    '2', 0,                     // wcChar5
    /* String 2 (Product) */
    0x18,                       // bLength
    USB_DESCRIPTOR_TYPE_STRING, // bDescriptorType
    'U', 0,                     // wcChar0
    'S', 0,                     // wcChar1
    'B', 0,                     // wcChar2
    '2', 0,                     // wcChar3
    'C', 0,                     // wcChar4
    'A', 0,                     // wcChar5
    'N', 0,                     // wcChar6
    '-', 0,                     // wcChar7
    'E', 0,                     // wcChar8
    'S', 0,                     // wcChar9
    'P', 0,                     // wcChar10
    /* String 3 (Serial Number) */
    0x1a,                       // bLength
    USB_DESCRIPTOR_TYPE_STRING, // bDescriptorType
    '0', 0,                     // wcChar0
    '1', 0,                     // wcChar1
    '2', 0,                     // wcChar2
    '3', 0,                     // wcChar3
    '4', 0,                     // wcChar4
    '5', 0,                     // wcChar5
    '0', 0,                     // wcChar0
    '1', 0,                     // wcChar1
    '2', 0,                     // wcChar2
    '3', 0,                     // wcChar3
    '4', 0,                     // wcChar4
    '5', 0,                     // wcChar5
    /* String 4 (Interface) */
    0x36,                       // bLength
    USB_DESCRIPTOR_TYPE_STRING, // bDescriptorType
    'E', 0,                     // wcChar0
    'S', 0,                     // wcChar1
    'P', 0,                     // wcChar2
    '3', 0,                     // wcChar3
    '2', 0,                     // wcChar4
    ' ', 0,                     // wcChar5
    'C', 0,                     // wcChar6
    'A', 0,                     // wcChar7
    'N', 0,                     // wcChar8
    'D', 0,                     // wcChar9
    'L', 0,                     // wcChar10
    'E', 0,                     // wcChar11
    'L', 0,                     // wcChar12
    'I', 0,                     // wcChar13
    'G', 0,                     // wcChar14
    'H', 0,                     // wcChar15
    'T', 0,                     // wcChar16
    ' ', 0,                     // wcChar17
    'F', 0,                     // wcChar18
    'I', 0,                     // wcChar19
    'R', 0,                     // wcChar20
    'A', 0,                     // wcChar21
    'W', 0,                     // wcChar22
    'A', 0,                     // wcChar23
    'R', 0,                     // wcChar24
    'E', 0,                     // wcChar25
    /* Device Qualifier */
    0x0a,
    USB_DESCRIPTOR_TYPE_DEVICE_QUALIFIER,
    0x10,
    0x02,
    0x00,
    0x00,
    0x00,
    0x40,
    0x01,
    0x00,
    /* End */
    0x00
};

__ALIGN_BEGIN uint8_t usbcan_bos_descriptor[] __ALIGN_END = {
    5,         // bLength
    USB_DESCRIPTOR_TYPE_BINARY_OBJECT_STORE,
    WBVAL(33), // wTotalLength
    1,         // bNumDeviceCaps
    /* WinUSB */
    28,                                    // bLength
    USB_DESCRIPTOR_TYPE_DEVICE_CAPABILITY, // bDescriptorType
    USB_BOS_CAPABILITY_PLATFORM,           // bDevCapabilityType
    0x00,                                  // bReserved
    0xDF, 0x60, 0xDD, 0xD8,
    0x89, 0x45, 0xC7, 0x4C,
    0x9C, 0xD2, 0x65, 0x9D,
    0x9E, 0x64, 0x8A, 0x9F, // PlatformCapabilityUUID
    /* CapabilityData (Descriptor Set Information) */
    0x00, 0x00, 0x03, 0x06,             // dwWindowsVersion
    WBVAL(USBCAN_MS_OS_20_DESC_LEN),    // wDescriptorSetTotalLength
    (uint8_t)USBCAN_WINUSB_VENDOR_CODE, // bVendorCode
    0,                                  // bAltEnumCode
};

__ALIGN_BEGIN uint8_t usbcan_msosv2_descriptor[] __ALIGN_END = {
    /* set header */
    WBVAL(10),                       // wLength
    WBVAL(WINUSB_SET_HEADER_DESCRIPTOR_TYPE),
    0x00, 0x00, 0x03, 0x06,          // dwWindowsVersion
    WBVAL(USBCAN_MS_OS_20_DESC_LEN), // wTotalLength
    /* compatible ID descriptor */
    WBVAL(20),                          // wLength
    WBVAL(WINUSB_FEATURE_COMPATIBLE_ID_TYPE),
    'W', 'I', 'N', 'U', 'S', 'B', 0, 0, // CompatibleID
    0, 0, 0, 0, 0, 0, 0, 0,             // SubCompatibleID
    /* registry property */
    WBVAL(132),                                // wLength
    WBVAL(WINUSB_FEATURE_REG_PROPERTY_TYPE),
    WBVAL(WINUSB_PROP_DATA_TYPE_REG_MULTI_SZ), // wPropertyDataType
    WBVAL(42),                                 // wPropertyNameLength
    'D', 0, 'e', 0, 'v', 0, 'i', 0, 'c', 0, 'e', 0,
    'I', 0, 'n', 0, 't', 0, 'e', 0, 'r', 0, 'f', 0, 'a', 0, 'c', 0, 'e', 0,
    'G', 0, 'U', 0, 'I', 0, 'D', 0, 's', 0, 0, 0, // PropertyName
    WBVAL(80),                                    // wPropertyDataLength
    0x7b, 0x00, 0x63, 0x00,                       /* property name: "{c15b4308-04d3-11e6-b3ea-6057189e6443}\0\0" */
    0x31, 0x00, 0x35, 0x00,
    0x62, 0x00, 0x34, 0x00,
    0x33, 0x00, 0x30, 0x00,
    0x38, 0x00, 0x2d, 0x00,
    0x30, 0x00, 0x34, 0x00,
    0x64, 0x00, 0x33, 0x00,
    0x2d, 0x00, 0x31, 0x00,
    0x31, 0x00, 0x65, 0x00,
    0x36, 0x00, 0x2d, 0x00,
    0x62, 0x00, 0x33, 0x00,
    0x65, 0x00, 0x61, 0x00,
    0x2d, 0x00, 0x36, 0x00,
    0x30, 0x00, 0x35, 0x00,
    0x37, 0x00, 0x31, 0x00,
    0x38, 0x00, 0x39, 0x00,
    0x65, 0x00, 0x36, 0x00,
    0x34, 0x00, 0x34, 0x00,
    0x33, 0x00, 0x7d, 0x00,
    0x00, 0x00, 0x00, 0x00
};

TU_VERIFY_STATIC(sizeof(usbcan_msosv2_descriptor) == USBCAN_MS_OS_20_DESC_LEN, "Incorrect size");

void usbcan_sreial_string_register()
{
    for (uint16_t i = 0; i < (MAC_BYTES * 2); i++) {
        usbcan_descriptor[SERIAL_STRING_INDEX + i * 2] = (uint8_t)serialDescriptor[i];
    }
}

static int
usbcan_class_interface_request_handler(struct usb_setup_packet *setup, uint8_t **data, uint32_t *len)
{
    USB_LOG_WRN("usbcan_class_interface_request_handler: "
                "bRequest 0x%02x\r\n",
                setup->bRequest);

    return -1;
}

static void usbcan_notify_handler(uint8_t event, void *arg)
{
    USB_LOG_WRN("usbcan notify handle: "
                "event 0x%02x\r\n",
                event);
    switch (event) {
        case USBD_EVENT_ERROR:

            break;
        case USBD_EVENT_RESET:

            break;
        case USBD_EVENT_SOF:

            break;
        case USBD_EVENT_CONNECTED:

            break;
        case USBD_EVENT_DISCONNECTED:

            break;
        case USBD_EVENT_SUSPEND:

            break;
        case USBD_EVENT_RESUME:

            break;
        case USBD_EVENT_CONFIGURED:

            break;
        case USBD_EVENT_SET_INTERFACE:

            break;
        case USBD_EVENT_UNKNOWN:

            break;

        default:
            break;
    }
}

int usbcan_class_endpoint_handler(struct usb_setup_packet *setup, uint8_t **data, uint32_t *len)
{
    USB_LOG_WRN("usbcan class endpoint handle: "
                "setup 0x%02x\r\n",
                setup->bRequest);
    return -1;
}

int my_abs1(int num)
{
    if (num < 0) {
        num = ~(num - 1);
    }
    return num;
}

int usbcan_vendor_handler(struct usb_setup_packet *setup, uint8_t **data, uint32_t *len)
{
    uint32_t param_u32;
    if ((setup->bmRequestType >> 7) == 0) { // host to device
        switch (setup->bRequest) {
            case GS_USB_BREQ_HOST_FORMAT: {
                memcpy(&param_u32, *data, sizeof(uint32_t));
                if (param_u32 == 0x0000beef) {
                    return 0;
                }
            } break;
            case GS_USB_BREQ_MODE: {
                if (setup->wValue < NUM_CAN_CHANNEL) {
                    struct gs_device_mode *mode = (struct gs_device_mode *)(*data);
                    can_channel_t *ch = channels[setup->wValue];
                    switch (mode->mode) {
                        case GS_CAN_MODE_RESET: {
                            int r = 0; // can0_disable();
                            if (r == 0) {
                                //  led_set_mode(led_mode_off, pdTICKS_TO_MS(2));
                                return 0;
                            } else {
                                break;
                            }
                        } break;
                        case GS_CAN_MODE_START: {
                            timestamps_enabled = (mode->flags & GS_CAN_MODE_HW_TIMESTAMP) != 0;
                            pad_pkts_to_max_pkt_size = (mode->flags & GS_CAN_MODE_PAD_PKTS_TO_MAX_PKT_SIZE) != 0;

                            int r = 0; //can0_enable((mode->flags & GS_CAN_MODE_LOOP_BACK) != 0,
                                       //   (mode->flags & GS_CAN_MODE_LISTEN_ONLY) != 0,
                                       // (mode->flags & GS_CAN_MODE_ONE_SHOT) != 0);
                            if (r == 0) {
                                //led_set_mode(led_mode_normal, pdTICKS_TO_MS(2));
                                return 0;
                            } else {
                                // led_set_mode(led_mode_off, pdTICKS_TO_MS(2));
                                break;
                            }
                        } break;
                        default:
                            break;
                    }
                }
                return -1;
            } break;
            case GS_USB_BREQ_BITTIMING: {
                if (setup->wValue < NUM_CAN_CHANNEL) {
                    struct gs_device_bittiming *timing = (struct gs_device_bittiming *)(*data);
                    can_channel_t *ch = channels[setup->wValue];
                    // int r = can0_set_bittiming(timing->brp, timing->phase_seg1, timing->phase_seg2, timing->sjw);
                    // if (r != 0) {
                    //     break;
                    // }
                    // r = can0_disable();
                    // if (r == 0) {
                    //     //  led_set_mode(led_mode_off, pdTICKS_TO_MS(2));
                    //     return 0;
                    // } else {
                    //     break;
                    // }
                    return 0;
                }
            } break;
            case GS_USB_BREQ_SET_USER_ID:
                break;
            case GS_USB_BREQ_IDENTIFY: {
                memcpy(&param_u32, *data, sizeof(param_u32));
                if (param_u32) {
                    //led_set_mode(led_mode_sequence, pdTICKS_TO_MS(10));
                } else {
                    can_channel_t *ch = channels[setup->wValue];
                    //led_set_mode((ch->state == 1) ? led_mode_normal : led_mode_off, pdTICKS_TO_MS(10));
                }
                return 0;
                break;
            }
            case GS_USB_BREQ_SET_TERMINATION: {
                memcpy(&param_u32, *data, sizeof(param_u32));
                if (setTermResistor(setup->wValue, param_u32) != TERM_UNSUPPORTED) {
                    return 0;
                }
            } break;
            default:
                //USB_LOG_WRN("Unhandled usbcab Class bRequest 0x%02x\r\n", setup->bRequest);
                break;
        }
        return -1;
    } else {
        switch (setup->bRequest) {
            case GS_USB_BREQ_HOST_FORMAT:
                break;
            case GS_USB_BREQ_MODE:
                break;
            case GS_USB_BREQ_BITTIMING:
                break;
            case GS_USB_BREQ_IDENTIFY:
                break;
            case GS_USB_BREQ_GET_TERMINATION:
                break;
            case GS_USB_BREQ_BT_CONST: // 4
            {
                memcpy(*data, &USBD_GS_CAN_btconst, sizeof(USBD_GS_CAN_btconst));
                *len = sizeof(USBD_GS_CAN_btconst);
                return 0;
            } break;
            case GS_USB_BREQ_DEVICE_CONFIG:                                   // 5
            {
                memcpy(*data, &USBD_GS_CAN_dconf, sizeof(USBD_GS_CAN_dconf)); // sizeof(USBD_GS_CAN_dconf)
                *len = sizeof(USBD_GS_CAN_dconf);
                return 0;
            } break;

            case GS_USB_BREQ_TIMESTAMP: {
                sof_timestamp_us = esp_timer_get_time();
                memcpy(*data, &sof_timestamp_us, sizeof(sof_timestamp_us));
                *len = sizeof(sof_timestamp_us);
                return 0;
            } break;
            case GS_USB_BREQ_GET_USER_ID:   // disabled
                break;
            case GS_USB_BREQ_SET_USER_ID:   // disabled
                break;
            case USBCAN_WINUSB_VENDOR_CODE: //cherryusb已经处理OS描述符的请求。
                break;
            default:
                USB_LOG_WRN("Unhandled usbcab Class bRequest 0x%02x\r\n", setup->bRequest);
                break;
        }
        return -1;
    }
}

void usbd_configure_done_callback(void)
{
    /* setup first out ep read transfer */
    //usbd_ep_start_read(USBCAN_EP_SEND, read_buffer, 2048);
}

struct usbd_interface *usbd_usbcan_init_intf(usbd_interface_t *intf)
{
    intf->class_interface_handler = usbcan_class_interface_request_handler;
    intf->class_endpoint_handler = usbcan_class_endpoint_handler;
    intf->vendor_handler = usbcan_vendor_handler;
    intf->notify_handler = usbcan_notify_handler;
    return intf;
}

struct usbd_endpoint winusb_out_ep = {
    .ep_addr = USBCAN_EP_SEND,
    .ep_cb = usbcan_out_callback
};

struct usbd_endpoint winusb_in_ep = {
    .ep_addr = USBCAN_EP_RECV,
    .ep_cb = usncan_in_callback
};
struct usb_msosv2_descriptor usbcan_msosv2_desc = {
    .compat_id = usbcan_msosv2_descriptor,
    .compat_id_len = USBCAN_MS_OS_20_DESC_LEN,
    .vendor_code = (USBCAN_WINUSB_VENDOR_CODE),
};
struct usb_bos_descriptor usbcan_bos_desc = {
    .string = usbcan_bos_descriptor,
    .string_len = sizeof(usbcan_bos_descriptor),
};

void winusb_init(void)
{
    usbd_msosv2_desc_register(&usbcan_msosv2_desc);
    usbd_bos_desc_register(&usbcan_bos_desc);
    usbd_desc_register(usbcan_descriptor);
    usbd_add_interface((usbd_usbcan_init_intf(&intf0)));
    usbd_add_endpoint(&winusb_out_ep);
    usbd_add_endpoint(&winusb_in_ep);

    usbd_initialize();
}