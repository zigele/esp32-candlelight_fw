/*
 * @Author       : 陈科进
 * @Date         : 2023-05-18 13:27:40
 * @LastEditors  : 陈科进
 * @LastEditTime : 2023-05-29 23:52:47
 * @FilePath     : \main\main.c
 * @Description  : ESP32 candlelight firmware object
 */

#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "usbd_core.h"
#include "usbd_can.h"
#include "others/led.h"
#include "driver/twai.h"
void InitSerialNumber(void)
{
    uint8_t m[MAC_BYTES] = {0};
    esp_err_t ret = esp_efuse_mac_get_default(m);
    if (ret != ESP_OK)
    {
        printf("Cannot read MAC address and set the device serial number");
    }
    else
    {
        printf("read MAC address and set the device serial number success.");
    }
    snprintf(serialDescriptor, sizeof(serialDescriptor),
             "%02X%02X%02X%02X%02X%02X", m[0], m[1], m[2], m[3], m[4], m[5]);

    usbcan_sreial_string_register();
}

uint8_t buff[32 * 64] = {0};
uint8_t buff_count = 0;

void tud_vendor_tx()
{
    while (1)
    {
        twai_status_info_t status_info;
        esp_err_t res = twai_get_status_info(&status_info);
        if (res == ESP_OK)
        {
            ESP_LOGI(__func__, "%d", status_info.state);
            ESP_LOGI("count12", "%d", count12);
        }
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
    vTaskDelete(NULL);
}
void app_main()
{
    //  led_init();
    InitSerialNumber();
    winusb_init();
    while (!usb_device_is_configured())
    {
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
    can0_init();

    // xTaskCreatePinnedToCore(tud_vendor_tx, "tud_vendor_tx_task", 4096, NULL, 2, NULL, 0);
    ESP_LOGI("2222", "3344");
    // xTaskCreatePinnedToCore(led_update_task, "led_update_task", 1000, NULL, 2, NULL, 1);
}