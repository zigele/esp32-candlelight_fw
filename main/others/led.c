/*

The MIT License (MIT)

Copyright (c) 2016 Hubert Denkmair

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

*/

#include "led.h"
#include "ws2812.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

static QueueHandle_t led_task_queue;

ws2812_pixel_t led_off_pixel = {
	.green = 0,
	.red = 0,
	.blue = 0,
};
ws2812_pixel_t led_boot_pixel = {
	.green = 0xa5,
	.red = 0xff,
	.blue = 0,
};
ws2812_pixel_t led_normal_pixel = {
	.green = 0x8b,
	.red = 0x22,
	.blue = 0x22,
};
ws2812_pixel_t led_gradient_pixel_1 = {
	.red = 0x2F,
	.green = 0x48,
	.blue = 0x58,
};
ws2812_pixel_t led_gradient_pixel_2 = {
	.red = 0x00,
	.green = 0x63,
	.blue = 0x7a,
};
ws2812_pixel_t led_gradient_pixel_3 = {
	.red = 0,
	.green = 0x80,
	.blue = 0x8e,
};
ws2812_pixel_t led_gradient_pixel_4 = {
	.red = 0x0,
	.green = 0x9d,
	.blue = 0x8e,
};
ws2812_pixel_t led_gradient_pixel_5 = {
	.red = 0x00,
	.green = 0xb8,
	.blue = 0x7a,
};
ws2812_pixel_t led_gradient_pixel_6 = {
	.red = 0x21,
	.green = 0xd0,
	.blue = 0x52,
};
void led_init()
{
	ws2812_init();
	led_task_queue = xQueueCreate(30, sizeof(ws2812_pixel_t));
}

void led_set_mode(led_mode_t state, uint16_t timeout)
{
	xQueueReset(led_task_queue);
	switch (state)
	{
	case led_mode_off:
		xQueueSend(led_task_queue, &led_off_pixel, timeout);
		break;
	case led_mode_boot:
		xQueueSend(led_task_queue, &led_boot_pixel, timeout);
		break;
	case led_mode_normal:
		xQueueSend(led_task_queue, &led_normal_pixel, timeout);
		break;
	case led_mode_sequence:
		xQueueSend(led_task_queue, &led_gradient_pixel_1, timeout);
		xQueueSend(led_task_queue, &led_gradient_pixel_2, timeout);
		xQueueSend(led_task_queue, &led_gradient_pixel_3, timeout);
		xQueueSend(led_task_queue, &led_gradient_pixel_4, timeout);
		xQueueSend(led_task_queue, &led_gradient_pixel_5, timeout);
		xQueueSend(led_task_queue, &led_gradient_pixel_6, timeout);
		break;

	default:
		break;
	}
}

void led_update_task(void *arg)
{
	while (1)
	{
		ws2812_pixel_t led_mode;
		xQueuePeek(led_task_queue, &led_mode, portMAX_DELAY); // 读取但是不删除
		ws2812_update(&led_mode);
		vTaskDelay(pdTICKS_TO_MS(100));
	}
	vTaskDelete(NULL);
}
