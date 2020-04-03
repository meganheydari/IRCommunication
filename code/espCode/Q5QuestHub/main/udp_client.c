#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"


//include uart communication
//#include "uart.h"
//include rxtx 38khz signal generation
#include "rxtx.h"
//include udp wifi connection to fob
//#include "udp.h"
//include serial connection from hub to rpi
#include "serial.h"

void app_main(void)
{
    // //initialize uart communication @ uart.h from port TX
    xTaskCreate(echo_task, "uart_echo_task", 1024, NULL, 10, NULL);

    // //initialize 38khz signal generation
    //nec_tx_init();

    // //initialize connection to webserver 
    // UDPHandler();
 
    //init serial connection to webserver 
    initSerial();
    serialHandler();

    // //from serial.h, set values to be sent to rpi
    // int fob_ID = 1;
    // int hub_ID = 1;
    // int code   = 1234;

    // //send this data once
    // sendSerial = true;
        
    

}
