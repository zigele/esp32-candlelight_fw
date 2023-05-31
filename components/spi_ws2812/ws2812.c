/** MIT licence

 Copyright (C) 2019 by Vu Nam https://github.com/vunam https://studiokoda.com

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights to
 use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 of the Software, and to permit persons to whom the Software is furnished to do
 so, subject to the following conditions: The above copyright notice and this
 permission notice shall be included in all copies or substantial portions of
 the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO
 EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES
 OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 DEALINGS IN THE SOFTWARE.

*/

#include <math.h>
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/i2s.h"
#include "ws2812.h"
i2s_config_t i2s_config = {
    .mode = I2S_MODE_MASTER | I2S_MODE_TX,
    .sample_rate = SAMPLE_RATE,
    .bits_per_sample = 16,
    .communication_format = I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB,
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
    .intr_alloc_flags = 0,
    .dma_buf_count = 4,
    .use_apll = false,
};

i2s_pin_config_t pin_config = {.bck_io_num = -1,
                               .ws_io_num = -1,
                               .data_out_num = I2S_DO_IO,
                               .data_in_num = -1};

static uint8_t out_buffer[LED_NUMBER * PIXEL_SIZE] = {0};
static uint8_t off_buffer[ZERO_BUFFER] = {0};
static uint16_t size_buffer;

static const uint16_t bitpatterns[4] = {0x88, 0x8e, 0xe8, 0xee};

void ws2812_init()
{
  size_buffer = LED_NUMBER * PIXEL_SIZE;

  i2s_config.dma_buf_len = size_buffer;

  i2s_driver_install(I2S_NUM, &i2s_config, 0, NULL);
  i2s_set_pin(I2S_NUM, &pin_config);
}

void ws2812_update(ws2812_pixel_t *pixels)
{
  size_t bytes_written = 0;

  for (uint16_t i = 0; i < LED_NUMBER; i++)
  {
    int loc = i * PIXEL_SIZE;

    out_buffer[loc] = bitpatterns[pixels[i].green >> 6 & 0x03];
    out_buffer[loc + 1] = bitpatterns[pixels[i].green >> 4 & 0x03];
    out_buffer[loc + 2] = bitpatterns[pixels[i].green >> 2 & 0x03];
    out_buffer[loc + 3] = bitpatterns[pixels[i].green & 0x03];

    out_buffer[loc + 4] = bitpatterns[pixels[i].red >> 6 & 0x03];
    out_buffer[loc + 5] = bitpatterns[pixels[i].red >> 4 & 0x03];
    out_buffer[loc + 6] = bitpatterns[pixels[i].red >> 2 & 0x03];
    out_buffer[loc + 7] = bitpatterns[pixels[i].red & 0x03];

    out_buffer[loc + 8] = bitpatterns[pixels[i].blue >> 6 & 0x03];
    out_buffer[loc + 9] = bitpatterns[pixels[i].blue >> 4 & 0x03];
    out_buffer[loc + 10] = bitpatterns[pixels[i].blue >> 2 & 0x03];
    out_buffer[loc + 11] = bitpatterns[pixels[i].blue & 0x03];
  }

  i2s_write(I2S_NUM, out_buffer, size_buffer, &bytes_written, portMAX_DELAY);
  i2s_write(I2S_NUM, off_buffer, ZERO_BUFFER, &bytes_written, portMAX_DELAY);
  vTaskDelay(pdMS_TO_TICKS(10));
  i2s_zero_dma_buffer(I2S_NUM);
}
void led_strip_hsv2rgb(uint32_t h, uint32_t s, uint32_t v, uint32_t *r, uint32_t *g, uint32_t *b)
{
  h %= 360; // h -> [0,360]
  uint32_t rgb_max = v * 2.55f;
  uint32_t rgb_min = rgb_max * (100 - s) / 100.0f;

  uint32_t i = h / 60;
  uint32_t diff = h % 60;

  // RGB adjustment amount by hue
  uint32_t rgb_adj = (rgb_max - rgb_min) * diff / 60;

  switch (i)
  {
  case 0:
    *r = rgb_max;
    *g = rgb_min + rgb_adj;
    *b = rgb_min;
    break;
  case 1:
    *r = rgb_max - rgb_adj;
    *g = rgb_max;
    *b = rgb_min;
    break;
  case 2:
    *r = rgb_min;
    *g = rgb_max;
    *b = rgb_min + rgb_adj;
    break;
  case 3:
    *r = rgb_min;
    *g = rgb_max - rgb_adj;
    *b = rgb_max;
    break;
  case 4:
    *r = rgb_min + rgb_adj;
    *g = rgb_min;
    *b = rgb_max;
    break;
  default:
    *r = rgb_max;
    *g = rgb_min;
    *b = rgb_max - rgb_adj;
    break;
  }
}