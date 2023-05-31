idf_component_register(SRCS 
    "main.c"

    "usb_cherry/addition/usb_cherry_platform.c"
    "usb_cherry/CherryUSB/class/usbcan/usbd_can.c"
    "usb_cherry/CherryUSB/core/usbd_core.c"
    "usb_cherry/CherryUSB/port/dwc2/usb_dc_dwc2.c"

    "can2/can.c"
    "others/led.c"
    "others/term_resistor.c"

    INCLUDE_DIRS
    "."

    "usb_cherry/CherryUSB/class/usbcan"
    "usb_cherry/CherryUSB/common"
    "usb_cherry/CherryUSB/core"
    "usb_cherry/CherryUSB/port/dwc2"

    "ring_buffer"
    "can2"
    "others"
)
