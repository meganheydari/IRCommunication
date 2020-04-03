#ifndef UART_H
#define UART_H

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include <string.h>
#include "button.h"

#define ECHO_TEST_TXD  26
#define ECHO_TEST_RXD  16 //CHANGE
#define ECHO_TEST_RTS  (UART_PIN_NO_CHANGE)
#define ECHO_TEST_CTS  (UART_PIN_NO_CHANGE)

#define BUF_SIZE (1024)




static void echo_task(void *arg)
{
    /* Configure parameters of an UART driver,
     * communication pins and install the driver */
    uart_config_t uart_config = {
        .baud_rate = 2400,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    uart_param_config(UART_NUM_1, &uart_config);
    uart_set_pin(UART_NUM_1, ECHO_TEST_TXD, ECHO_TEST_RXD, ECHO_TEST_RTS, ECHO_TEST_CTS);
    uart_driver_install(UART_NUM_1, BUF_SIZE * 2, 0, 0, NULL, 0);
    
    uart_set_line_inverse(UART_NUM_1, UART_INVERSE_RXD);

    // Configure a temporary buffer for the incoming data
    uint8_t *data1 = (const char *) "{\"fob_ID\" : \"03\", \"code\" : \"1236\"}";
    int readLen = strlen((const char *) data1);
    uart_write_bytes(UART_NUM_1, (const char *) data1, readLen);

    uint8_t *data = (uint8_t *) malloc(readLen);


    while (1) {
        if (buttonPress == 1) {
            uart_write_bytes(UART_NUM_1, (const char *) data1, readLen);
            printf("Writing Data: %s\n", data1);
        }
        vTaskDelay(1000 / portTICK_RATE_MS);
    }
}


#endif

