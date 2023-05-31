/*
 * @Author       : 陈科进
 * @Date         : 2023-05-18 20:21:33
 * @LastEditors  : 陈科进
 * @LastEditTime : 2023-05-19 19:50:09
 * @FilePath     : \main\usb_cherry\addition\usb_cherry_platform.c
 * @Description  : ESP32 candlelight firmware object
 */
#include "esp_intr_alloc.h"
#include "soc/usb_periph.h"
#include "soc/periph_defs.h"
#include "esp_private/usb_phy.h"
#include "hal/usb_hal.h"
#include "soc/usb_periph.h"
#include "driver/gpio.h"
#include "driver/periph_ctrl.h"
#include <stdio.h>

uint32_t SystemCoreClock = 160 * 1000000; // dwc2 的一个参数

static intr_handle_t interrupt_handle_ps; // esp32 中断要用

static void ConfigureUsbPins(usb_hal_context_t *usb)
{
    /* usb_periph_iopins currently configures USB_OTG as USB Device.
     * Introduce additional parameters in usb_hal_context_t when adding support
     * for USB Host.
     */
    for (const usb_iopin_dsc_t *iopin = usb_periph_iopins; iopin->pin != -1; ++iopin)
    {
        if ((usb->use_external_phy) || (iopin->ext_phy_only == 0))
        {
            gpio_pad_select_gpio(iopin->pin);
            if (iopin->is_output)
            {
                gpio_matrix_out(iopin->pin, iopin->func, false, false);
            }
            else
            {
                gpio_matrix_in(iopin->pin, iopin->func, false);
                gpio_pad_input_enable(iopin->pin);
            }
            gpio_pad_unhold(iopin->pin);
        }
    }
    if (!usb->use_external_phy)
    {
        gpio_set_drive_capability(USBPHY_DM_NUM, GPIO_DRIVE_CAP_3);
        gpio_set_drive_capability(USBPHY_DP_NUM, GPIO_DRIVE_CAP_3);
    }
}
void usb_dc_low_level_init(void)
{
    periph_module_reset(PERIPH_USB_MODULE);
    periph_module_enable(PERIPH_USB_MODULE);

    usb_hal_context_t hal = {
        .use_external_phy = false};
    usb_hal_init(&hal);
    ConfigureUsbPins(&hal);
    void usb_interrupt_enable();

    // esp32s2 phy
    static usb_phy_handle_t handle_s;
    usb_phy_config_t config_s = {
        .controller = USB_PHY_CTRL_OTG,
        .otg_mode = USB_OTG_MODE_DEVICE,
        .target = USB_PHY_TARGET_INT};
    usb_new_phy(&config_s, &handle_s);

    usb_interrupt_enable();
}

// usb 中断
static void usb_interrupt_cb(void *arg_pv)
{
    extern void OTG_FS_IRQHandler(); // dwc2 的中断处理函数

    OTG_FS_IRQHandler();
}

// usb 中断开启
void usb_interrupt_enable()
{
    esp_intr_alloc(ETS_USB_INTR_SOURCE, ESP_INTR_FLAG_LOWMED, (intr_handler_t)usb_interrupt_cb, 0, &interrupt_handle_ps);
}

// usb 中断关闭
void usb_interrupt_disable()
{
    esp_intr_free(interrupt_handle_ps);
}