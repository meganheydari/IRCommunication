#ifndef UDP_H
#define UDP_H

// startudp
#include <string.h>
#include <sys/param.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "tcpip_adapter.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>
#include "protocol_examples_common.h"
// endudp

// start udp
u16_t CONFIGPORT = 8080;
const char* IP = "192.168.1.131";

#ifdef CONFIG_EXAMPLE_IPV4
#define HOST_IP_ADDR IP
#else
#define HOST_IP_ADDR CONFIG_EXAMPLE_IPV6_ADDR
#endif
#define PORT CONFIGPORT

static const char *TAG = "example";
static const char *payload = "ACK";
static const char *initPayload = "leila";
TaskHandle_t  UDPHandle = NULL;
int authenticated = 0;
// end udp

// main logic for udp connection to the webserver
static void udp_client_task(void *pvParameters)
{
    char rx_buffer[128];
    char addr_str[128];
    int addr_family;
    int ip_protocol;

    while (1) {

        #ifdef CONFIG_EXAMPLE_IPV4
                struct sockaddr_in dest_addr;
                dest_addr.sin_addr.s_addr = inet_addr(HOST_IP_ADDR);
                dest_addr.sin_family = AF_INET;
                dest_addr.sin_port = htons(PORT);
                addr_family = AF_INET;
                ip_protocol = IPPROTO_IP;
                inet_ntoa_r(dest_addr.sin_addr, addr_str, sizeof(addr_str) - 1);
        #else // IPV6
                struct sockaddr_in6 dest_addr;
                inet6_aton(HOST_IP_ADDR, &dest_addr.sin6_addr);
                dest_addr.sin6_family = AF_INET6;
                dest_addr.sin6_port = htons(PORT);
                addr_family = AF_INET6;
                ip_protocol = IPPROTO_IPV6;
                inet6_ntoa_r(dest_addr.sin6_addr, addr_str, sizeof(addr_str) - 1);
        #endif

        int sock = socket(addr_family, SOCK_DGRAM, ip_protocol);
        if (sock < 0) {
            ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
            break;
        }
        ESP_LOGI(TAG, "Socket created, sending to %s:%d", HOST_IP_ADDR, PORT);

        int err = sendto(sock, initPayload, strlen(initPayload), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
            if (err < 0) {
                ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
                break;
            }
        ESP_LOGI(TAG, "Init message sent");
            
        while (1) {
            
        
            struct sockaddr_in source_addr; // Large enough for both IPv4 or IPv6
            socklen_t socklen = sizeof(source_addr);
            int len = recvfrom(sock, rx_buffer, sizeof(rx_buffer) - 1, 0, (struct sockaddr *)&source_addr, &socklen);

            // Error occurred during receiving
            if (len < 0) {
                ESP_LOGE(TAG, "recvfrom failed: errno %d", errno);
                break;
            }
            // Data received
            else {
                rx_buffer[len] = 0; // Null-terminate whatever we received and treat like a string
                printf("%s\n", rx_buffer);
                //printf("waiting on a signal.");


                if (strcmp(rx_buffer,"Authenticated") == 0) {
                    // Authenticated LOGIC
                    printf("Authenticated\n");
                    authenticated = 1;
                 }

                if (strcmp(rx_buffer,"Denied") == 0) {
                    // STOP LOGIC
                    printf("Denied\n");
                    authenticated = 0;
                }

                ESP_LOGI(TAG, "Received %d bytes from %s:", len, addr_str);
                ESP_LOGI(TAG, "%s", rx_buffer);
            }

            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }

        if (sock != -1) {
            ESP_LOGE(TAG, "Shutting down socket and restarting...");
            shutdown(sock, 0);
            close(sock);
        }
    }
    UDPHandle = NULL;
    vTaskDelete(UDPHandle);
}

// handles creation and deletion of udp connection to webserver
void UDPHandler()
{
	//initialize
    ESP_ERROR_CHECK(nvs_flash_init());
    tcpip_adapter_init();
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_ERROR_CHECK(example_connect());

    printf("running udp handler\n");
    if(UDPHandle != NULL)
    {
        vTaskDelete(UDPHandle);
        UDPHandle = NULL;
    }
    xTaskCreate(udp_client_task, "udp_client_task", 4096, NULL, 5, &UDPHandle);
    configASSERT(UDPHandle);
}

#endif