/*
 * @Author       : 陈科进
 * @Date         : 2023-05-13 19:50:06
 * @LastEditors  : 陈科进
 * @LastEditTime : 2023-05-13 19:51:57
 * @FilePath     : \main\others\led.h
 * @Description  : ESP32 candlelight firmware object
 */
#ifndef __LED_H__
#define __LED_H__
#include <string.h>

typedef enum
{
    led_mode_off,
    led_mode_boot,
    led_mode_normal,
    led_mode_sequence
} led_mode_t;

void led_set_mode(led_mode_t state, uint16_t timeout);
void led_update_task(void *arg);
void led_init();
#endif
