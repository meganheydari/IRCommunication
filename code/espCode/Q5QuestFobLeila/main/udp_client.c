#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"


//include uart communication
#include "uart.h"
//include rxtx 38khz signal generation
#include "rxtx.h"
// //include udp wifi connection to fob
#include "udp.h"
//include serial connection from hub to rpi
//#include "serial.h"
//include button press logic
#include "button.h"

void app_main(void)
{
    // refer to uart.h to set fob id and code
    // refer to udp.h and set initPayload to your name

    //initialize uart communication @ uart.h from port TX
    xTaskCreate(echo_task, "uart_echo_task", 1024, NULL, 10, NULL);

    //initialize 38khz signal generation
    nec_tx_init();

    // //initialize connection to webserver 
    UDPHandler();
    run_button();

}
