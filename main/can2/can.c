#include "can.h"
#include "esp_log.h"
#include "esp_system.h"
#include "usb_can.h"
#define TX_GPIO_NUM 4
#define RX_GPIO_NUM 6

portMUX_TYPE myMutex = portMUX_INITIALIZER_UNLOCKED;
SemaphoreHandle_t xSemaphore = NULL;

static twai_timing_config_t t_config = {.brp = 8, .tseg_1 = 15, .tseg_2 = 4, .sjw = 3, .triple_sampling = false}; // 500k
static twai_filter_config_t f_config = {.acceptance_code = 0, .acceptance_mask = 0xFFFFFFFF, .single_filter = true};
static twai_general_config_t g_config = {.mode = TWAI_MODE_NORMAL, .tx_io = TX_GPIO_NUM, .rx_io = RX_GPIO_NUM, .clkout_io = TWAI_IO_UNUSED, .bus_off_io = TWAI_IO_UNUSED, .tx_queue_len = 500, .rx_queue_len = 500, .alerts_enabled = TWAI_ALERT_NONE, .clkout_divider = 0, .intr_flags = ESP_INTR_FLAG_LEVEL1};
#define CAN_TX_FIFO_BUFFER_LEN 10000
#define CAN_RX_FIFO_BUFFER_LEN 10000
#define BIT_RATE_COUNT 14
uint32_t count12 = 0;
TaskHandle_t can0_receive_task_handle = NULL;
TaskHandle_t can0_transmit_task_handle = NULL;
static const uint32_t bit_rate_index[BIT_RATE_COUNT] = {1 * 1000, 5 * 1000, 10 * 1000, 12 * 1200, 16 * 1000, 20 * 1000, 25 * 1000, 50 * 1000, 100 * 1000, 125 * 1000, 250 * 1000, 500 * 1000, 800 * 1000, 1000 * 1000};
twai_timing_config_t get_can_timing_config()
{
    return t_config;
}

void can_timing_config_init(uint32_t brp, uint8_t tseg_1, uint8_t tseg_2, uint8_t sjw, bool triple_sampling)
{
    t_config.brp = brp;
    t_config.tseg_1 = tseg_1;
    t_config.tseg_2 = tseg_2;
    t_config.sjw = sjw;
    t_config.triple_sampling = triple_sampling; // 250K
}
void can_filter_code_init(uint32_t accept_code, uint32_t accept_mask, bool single_filter)
{

    f_config.acceptance_code = accept_code;
    f_config.acceptance_mask = accept_mask;
    f_config.single_filter = single_filter;
}

void can_driver_init()
{
    g_config.mode = TWAI_MODE_NORMAL;
    g_config.tx_io = TX_GPIO_NUM;
    g_config.rx_io = RX_GPIO_NUM;
    g_config.clkout_io = TWAI_IO_UNUSED;
    g_config.bus_off_io = TWAI_IO_UNUSED;
    g_config.tx_queue_len = CAN_TX_FIFO_BUFFER_LEN;
    g_config.rx_queue_len = CAN_RX_FIFO_BUFFER_LEN;
    g_config.alerts_enabled = TWAI_ALERT_NONE;
    g_config.clkout_divider = 0;
    g_config.intr_flags = ESP_INTR_FLAG_LEVEL1;
}

int my_abs(int num)
{
    if (num < 0)
    {
        num = ~(num - 1);
    }
    return num;
}
uint32_t bit_rate_index_match(uint16_t brp, uint8_t phase_seg1, uint8_t phase_seg2, uint8_t sjw)
{
    uint32_t bitRate = CAN_CLOCK_SPEED / (phase_seg1 + phase_seg2 + 1) / brp;
    uint32_t bias = 0;
    uint8_t min_index = 0;
    uint32_t min_value = my_abs(bit_rate_index[0] - bitRate);
    for (size_t i = 0; i < BIT_RATE_COUNT; i++)
    {
        bias = my_abs(bit_rate_index[i] - bitRate);
        if (bias < min_value)
        {
            min_value = bias;
            min_index = i;
        }
    }
    return bit_rate_index[min_index];
}

int can0_set_bittiming(uint16_t brp, uint8_t phase_seg1, uint8_t phase_seg2, uint8_t sjw)
{

    uint32_t bitRate = bit_rate_index_match(brp, phase_seg1, phase_seg2, sjw); // CAN_CLOCK_SPEED / (phase_seg1 + phase_seg2 + 1) / brp;
    switch (bitRate)
    {
    case 16 * 1000:
        t_config.brp = 200;
        t_config.tseg_1 = 16;
        t_config.tseg_2 = 8;
        t_config.sjw = 3;
        t_config.triple_sampling = false;
        break;
    case 20 * 1000:
        t_config.brp = 200;
        t_config.tseg_1 = 15;
        t_config.tseg_2 = 4;
        t_config.sjw = 3;
        t_config.triple_sampling = false;
        break;
    case 25 * 1000:
        t_config.brp = 128;
        t_config.tseg_1 = 16;
        t_config.tseg_2 = 8;
        t_config.sjw = 3;
        t_config.triple_sampling = false;
        break;
    case 50 * 1000:
        t_config.brp = 80;
        t_config.tseg_1 = 15;
        t_config.tseg_2 = 4;
        t_config.sjw = 3;
        t_config.triple_sampling = false;
        break;
    case 100 * 1000:
        t_config.brp = 40;
        t_config.tseg_1 = 15;
        t_config.tseg_2 = 4;
        t_config.sjw = 3;
        t_config.triple_sampling = false;
        break;
    case 125 * 1000:
        t_config.brp = 32;
        t_config.tseg_1 = 15;
        t_config.tseg_2 = 4;
        t_config.sjw = 3;
        t_config.triple_sampling = false;
        break;
    case 250 * 1000:
        t_config.brp = 16;
        t_config.tseg_1 = 15;
        t_config.tseg_2 = 4;
        t_config.sjw = 3;
        t_config.triple_sampling = false;
        break;
    case 500 * 1000:
        t_config.brp = 8;
        t_config.tseg_1 = 15;
        t_config.tseg_2 = 4;
        t_config.sjw = 3;
        t_config.triple_sampling = false;
        break;
    case 800 * 1000:
        t_config.brp = 4;
        t_config.tseg_1 = 16;
        t_config.tseg_2 = 8;
        t_config.sjw = 3;
        t_config.triple_sampling = false;
        break;
    case 1000 * 1000:
        t_config.brp = 4;
        t_config.tseg_1 = 15;
        t_config.tseg_2 = 4;
        t_config.sjw = 3;
        t_config.triple_sampling = false;
        break;
    default:
        t_config.brp = 8;
        t_config.tseg_1 = 15;
        t_config.tseg_2 = 4;
        t_config.sjw = 3;
        t_config.triple_sampling = false;
        return -1;
        break;
    }
    return 0;
}

void IRAM_ATTR usbcan_out_callback(uint8_t ep, uint32_t nbytes)
{

    // USB_LOG_RAW("actual out len:%d\r\n", nbytes);
}
BaseType_t xHigherPriorityTaskWoken;

void IRAM_ATTR usncan_in_callback(uint8_t ep, uint32_t nbytes)
{
    //  taskENTER_CRITICAL(&myMutex);
    // USB_LOG_RAW("actual in len:%d\r\n", nbytes);
    if ((nbytes % USBCAN_MAX_PACKET_SIZE) == 0 && nbytes)
    {
        usbd_ep_start_write(USBCAN_EP_SEND, NULL, 0);
    }
    else
    {
        xHigherPriorityTaskWoken = pdFALSE;
        xSemaphoreGiveFromISR(xSemaphore, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
    // taskEXIT_CRITICAL(&myMutex);
}
void IRAM_ATTR can0_receive_task(void *arg)
{
    gs_host_frame_t frame;
    frame.echo_id = 0xFFFFFFFF; // not an echo frame
    frame.channel = 0;
    frame.flags = 0;
    twai_message_t can_mes;
    frame.reserved = 0;
    uint8_t frame_len = sizeof(gs_host_frame_t) - 4;
    while (1)
    {
        twai_receive(&can_mes, portMAX_DELAY);
        frame.can_id = can_mes.identifier;
        frame.can_dlc = can_mes.data_length_code;
        memcpy(frame.data, can_mes.data, can_mes.data_length_code);
        // frame.timestamp_us = esp_timer_get_time();
        // count12++;
        xHigherPriorityTaskWoken = pdFALSE;
        xSemaphoreTakeFromISR(xSemaphore, &xHigherPriorityTaskWoken);
        usbd_ep_start_write(USBCAN_EP_SEND, &frame, frame_len);
    }
    // vTaskDelay(10 / portTICK_PERIOD_MS);
    vTaskDelete(NULL);
}

void IRAM_ATTR can0_transmit_task(void *arg)
{
    while (1)
    {
        gs_host_frame_t frame;
        xQueueReceive(txFromHostQueue, &frame, portMAX_DELAY);
        if (frame.echo_id == 0xFFFFFFFF)
        {
            twai_message_t can_mes;
            can_mes.identifier = frame.can_id;
            can_mes.data_length_code = frame.can_dlc;
            can_mes.flags = 0;
            can_mes.data[0] = frame.data[0];
            can_mes.data[1] = frame.data[1];
            can_mes.data[2] = frame.data[2];
            can_mes.data[3] = frame.data[3];
            can_mes.data[4] = frame.data[4];
            can_mes.data[5] = frame.data[5];
            can_mes.data[6] = frame.data[6];
            can_mes.data[7] = frame.data[7];
            if (twai_transmit(&can_mes, portMAX_DELAY) == ESP_OK)
            {
                xQueueSend(rxFromCANQueue, &can_mes, portMAX_DELAY);
            }
            // vTaskDelay(pdMS_TO_TICKS(10));
        }
    }
    vTaskDelete(NULL);
}
int can0_enable(bool loop_back, bool listen_only, bool one_shot)
{
    // can0_disable();
    g_config.mode = TWAI_MODE_NORMAL;
    if (loop_back)
    {
        g_config.mode = TWAI_MODE_NO_ACK;
    }
    if (listen_only)
    {
        g_config.mode = TWAI_MODE_LISTEN_ONLY;
    }
    if (one_shot)
    {
    }
    if ((twai_driver_install(&g_config, &t_config, &f_config) != ESP_OK))
    {
        return -1;
    }
    if (twai_start() != ESP_OK)
    {
        return -1;
    }
    if (can0_receive_task_handle == NULL)
    {
        xTaskCreatePinnedToCore(can0_receive_task, "can0_receive_task", 4096, NULL, RX_TASK_PRIO, &can0_receive_task_handle, tskNO_AFFINITY);
    }
    else
    {
    }
    if (can0_transmit_task_handle == NULL)
    {
        xTaskCreatePinnedToCore(can0_receive_task, "can0_receive_task", 4096, NULL, RX_TASK_PRIO, &can0_receive_task_handle, tskNO_AFFINITY);
    }
    else
    {
        // xTaskCreatePinnedToCore(can0_transmit_task, "can0_transmit_task", 4096, NULL, RX_TASK_PRIO, &can0_transmit_task_handle, 1);
    }
    return 0;
}
int can0_disable()
{
    esp_err_t res = ESP_OK;
    res = twai_stop();
    res = twai_driver_uninstall();
    xQueueReset(rxFromCANQueue);
    xQueueReset(txFromHostQueue);
    // if (can0_receive_task_handle != NULL)
    // {
    //     vTaskSuspend(can0_receive_task_handle);
    // }
    // if (can0_transmit_task_handle != NULL)
    // {
    //     vTaskSuspend(can0_transmit_task_handle);
    // }

    return 0;
}

int can0_init(void)
{
    xSemaphore = xSemaphoreCreateBinary();
    xSemaphoreGive(xSemaphore);
    rxFromCANQueue = xQueueCreate(100, sizeof(twai_message_t));
    txFromHostQueue = xQueueCreate(100, sizeof(gs_host_frame_t));
    can_timing_config_init(4, 15, 4, 3, false);
    can_filter_code_init(0, 0xFFFFFFFF, true);
    can_driver_init();
    can0_enable(false, false, false);
    return 0;
}
bool can0_get_state(twai_state_t *ts)
{
    esp_err_t res = 0xff;
    twai_status_info_t status_info;
    while (res != ESP_OK)
    {
        res = twai_get_status_info(&status_info);
        if (res == ESP_OK)
        {
            *ts = status_info.state;
            return true;
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
    return false;
}

uint32_t can0_get_error_status()
{
    return 1;
}
bool status_is_active(uint32_t err)
{
    if (err == TWAI_STATE_RUNNING)
    {
        return true;
    }
    else if (err == TWAI_STATE_BUS_OFF)
    {
        return false;
    }
    else if (err == TWAI_STATE_RECOVERING)
    {
        return false;
    }
    else
    {
        return false;
    }
}
bool can0_parse_error_status(uint32_t err, uint32_t last_err, struct gs_host_frame *frame)
{
    return true;
}