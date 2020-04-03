#ifndef SERIAL_H
#define SERIAL_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "string.h"
#include "driver/gpio.h"
#include <stdbool.h> 


static const int RX_BUF_SIZE = 1024;
TaskHandle_t  SerialHandle = NULL;
bool sendSerial = false;

#define TXD_PIN (GPIO_NUM_4)
#define RXD_PIN (GPIO_NUM_5)

void init(void) {
    const uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    uart_param_config(UART_NUM_1, &uart_config);
    uart_set_pin(UART_NUM_1, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    // We won't use a buffer for sending data.
    uart_driver_install(UART_NUM_1, RX_BUF_SIZE * 2, 0, 0, NULL, 0);

    static const char *TX_TASK_TAG = "TX_TASK";
    esp_log_level_set(TX_TASK_TAG, ESP_LOG_INFO);
}

char * append(char * string1, char * string2)
{
    char * result = NULL;
    asprintf(&result, "%s%s", string1, string2);
    return result;
}

static void tx_task(void *arg)
{

    while (1) {
        if (sendSerial) {
            printf("{\"fob_ID\": \"%d\", \"hub_ID\": \"%d\", \"code\": \"%d\"}\n", 1, 2, 3);
            sendSerial = false;
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
    SerialHandle = NULL;
    vTaskDelete(SerialHandle);
}

void initSerial() {
    printf("init serial\n");
    init();
}

void serialHandler() {
    printf("Running serial handler.\n");
    if(SerialHandle != NULL)
    {
        vTaskDelete(SerialHandle);
        SerialHandle = NULL;
    }

    xTaskCreate(tx_task, "tx_task", 1024*2, NULL, configMAX_PRIORITIES-1, &SerialHandle);
    configASSERT(SerialHandle);
}

#endif
