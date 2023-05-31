/*
 * @Author       : 陈科进
 * @Date         : 2023-05-03 10:56:01
 * @LastEditors  : 陈科进
 * @LastEditTime : 2023-05-03 11:01:56
 * @FilePath     : \components\spi_ws2812\ws2812.h
 * @Description  : ESP32 WIFI CAN object
 * Copyright 2023 OBKoro1, All Rights Reserved.
 * 2023-05-03 10:56:01
 */

#ifndef WS2812_H
#define WS2812_H

#include <stdbool.h>
#include <stdint.h>

#define LED_NUMBER 1
#define PIXEL_SIZE 12 // each colour takes 4 bytes
#define SAMPLE_RATE (93750)
#define ZERO_BUFFER 48
#define I2S_NUM (0)
#define I2S_DO_IO (48)
#define I2S_DI_IO (-1)

typedef struct
{
  uint8_t green;
  uint8_t red;
  uint8_t blue;
} ws2812_pixel_t;

void ws2812_init();

void ws2812_update(ws2812_pixel_t *pixels);

#endif
