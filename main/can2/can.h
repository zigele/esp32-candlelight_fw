/*
 * @Author       : 陈科进
 * @Date         : 2023-05-13 19:59:17
 * @LastEditors  : 陈科进
 * @LastEditTime : 2023-05-29 20:52:58
 * @FilePath     : \main\can2\can.h
 * @Description  : ESP32 candlelight firmware object
 */

#ifndef __CAN_H__
#define __CAN_H__
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/twai.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#define RX_TASK_PRIO 14
#define TX_TASK_PRIO 11
#define CAN_CLOCK_SPEED 48 * 1000 * 1000
#define NUM_CAN_CHANNEL 1
QueueHandle_t txFromHostQueue;
QueueHandle_t rxFromCANQueue;
SemaphoreHandle_t xSemaphore;
typedef struct __packed __aligned(4) gs_host_frame
{
    uint32_t echo_id;
    uint32_t can_id;

    uint8_t can_dlc;
    uint8_t channel;
    uint8_t flags;
    uint8_t reserved;

    uint8_t data[8];

    uint32_t timestamp_us;
}
gs_host_frame_t;

struct __packed __aligned(4) gs_host_frame_canfd
{
    uint32_t echo_id;
    uint32_t can_id;

    uint8_t can_dlc;
    uint8_t channel;
    uint8_t flags;
    uint8_t reserved;

    uint8_t data[64];
};

struct __packed __aligned(4) gs_tx_context
{
    struct gs_can *dev;
    unsigned int echo_id;
};
uint32_t count12;

int can0_init(void);
int can0_disable();
int can0_enable(bool loop_back, bool listen_only, bool one_shot);
int can0_set_bittiming(uint16_t brp, uint8_t phase_seg1, uint8_t phase_seg2, uint8_t sjw);
TaskHandle_t can0_receive_task_handle;
TaskHandle_t can0_transmit_task_handle;
twai_timing_config_t get_can_timing_config();
#endif