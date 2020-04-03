#ifndef BUTTON_H
#define BUTTON_H

// def for button
#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "udp.h"
// end def for button

// def for LED
#include <stdio.h>
#include "driver/ledc.h"
#include "esp_err.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "driver/uart.h"
#include "esp_vfs_dev.h"
// end def

//def for LED
#define LEDC_HS_TIMER          LEDC_TIMER_0
#define LEDC_HS_MODE           LEDC_HIGH_SPEED_MODE
#define LEDC_HS_CH0_GPIO       (12)
#define LEDC_HS_CH0_CHANNEL    LEDC_CHANNEL_1
#define LEDC_HS_CH1_GPIO       (27)
#define LEDC_HS_CH1_CHANNEL    LEDC_CHANNEL_0

#define LEDC_LS_TIMER          LEDC_TIMER_1
#define LEDC_LS_MODE           LEDC_LOW_SPEED_MODE
#define LEDC_LS_CH2_GPIO       (12)
#define LEDC_LS_CH2_CHANNEL    LEDC_CHANNEL_0
#define LEDC_LS_CH3_GPIO       (0)
#define LEDC_LS_CH3_CHANNEL    LEDC_CHANNEL_0

#define LEDC_TEST_CH_NUM       (4)
#define LEDC_TEST_DUTY         (4000)
#define LEDC_TEST_FADE_TIME    (3000)

// end def for led

// def for button
#define DEFAULT_VREF    1100        //Use adc2_vref_to_gpio() to obtain a better estimate
#define NO_OF_SAMPLES   1          //Multisampling

static esp_adc_cal_characteristics_t *adc_chars;
static const adc_channel_t channel = ADC_CHANNEL_6;     //GPIO34 if ADC1, GPIO14 if ADC2
static const adc_atten_t atten = ADC_ATTEN_DB_0;
static const adc_unit_t unit = ADC_UNIT_1;
int buttonPress = 0;

TaskHandle_t  LED = NULL;
// end def for button

static void toggle_LED_task(void *arg)
{
    // initialize led confg
    int ch2 = 0;
    int ch;

        ledc_timer_config_t ledc_timer = {
            .duty_resolution = LEDC_TIMER_13_BIT, // resolution of PWM duty
            .freq_hz = 5000,                      // frequency of PWM signal
            .speed_mode = LEDC_HS_MODE,           // timer mode
            .timer_num = LEDC_HS_TIMER,            // timer index
            .clk_cfg = LEDC_AUTO_CLK,              // Auto select the source clock
        };
        ledc_timer_config(&ledc_timer);

        ledc_timer.speed_mode = LEDC_LS_MODE;
        ledc_timer.timer_num = LEDC_LS_TIMER;
        ledc_timer_config(&ledc_timer);

        ledc_channel_config_t ledc_channel[LEDC_TEST_CH_NUM] = {
            {
                .channel    = LEDC_HS_CH0_CHANNEL,
                .duty       = 0,
                .gpio_num   = LEDC_HS_CH0_GPIO,
                .speed_mode = LEDC_HS_MODE,
                .hpoint     = 0,
                .timer_sel  = LEDC_HS_TIMER
            },
            {
                .channel    = LEDC_HS_CH1_CHANNEL,
                .duty       = 0,
                .gpio_num   = LEDC_HS_CH1_GPIO,
                .speed_mode = LEDC_HS_MODE,
                .hpoint     = 0,
                .timer_sel  = LEDC_HS_TIMER
            },
            {
                .channel    = LEDC_LS_CH2_CHANNEL,
                .duty       = 0,
                .gpio_num   = LEDC_LS_CH2_GPIO,
                .speed_mode = LEDC_LS_MODE,
                .hpoint     = 0,
                .timer_sel  = LEDC_LS_TIMER
            },
            {
                .channel    = LEDC_LS_CH3_CHANNEL,
                .duty       = 0,
                .gpio_num   = LEDC_LS_CH3_GPIO,
                .speed_mode = LEDC_LS_MODE,
                .hpoint     = 0,
                .timer_sel  = LEDC_LS_TIMER
            },
        };

         
        ch = 1;

        ledc_channel_config(&ledc_channel[ch]);
        ledc_channel_config(&ledc_channel[ch2]);

        ledc_fade_func_install(0);

    // === end init

    while(1) {
        if (buttonPress == 1) 
        {
            ledc_set_fade_with_time(ledc_channel[ch].speed_mode, ledc_channel[ch].channel, 11 * 444, 1000);
            ledc_fade_start(ledc_channel[ch].speed_mode, ledc_channel[ch].channel, LEDC_FADE_NO_WAIT);
        }
        if (authenticated == 1) 
        {
            ledc_set_fade_with_time(ledc_channel[ch2].speed_mode, ledc_channel[ch2].channel, 11 * 444, 1000);
            ledc_fade_start(ledc_channel[ch2].speed_mode, ledc_channel[ch2].channel, LEDC_FADE_NO_WAIT);
        }
        else {
            ledc_set_fade_with_time(ledc_channel[ch2].speed_mode, ledc_channel[ch2].channel, 0, 1000);
            ledc_fade_start(ledc_channel[ch2].speed_mode, ledc_channel[ch2].channel, LEDC_FADE_NO_WAIT);
        }

            vTaskDelay(1000 / portTICK_PERIOD_MS);
        if (buttonPress == 1) 
        {
            ledc_set_fade_with_time(ledc_channel[ch].speed_mode, ledc_channel[ch].channel, 1 * 444, 1000);
            ledc_fade_start(ledc_channel[ch].speed_mode, ledc_channel[ch].channel, LEDC_FADE_NO_WAIT);
            
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        
        }
    
    ledc_set_fade_with_time(ledc_channel[ch].speed_mode, ledc_channel[ch].channel, 0, 1000);
    ledc_fade_start(ledc_channel[ch].speed_mode, ledc_channel[ch].channel, LEDC_FADE_NO_WAIT);


    vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

    ledc_set_fade_with_time(ledc_channel[ch].speed_mode, ledc_channel[ch].channel, 0, 1000);
    ledc_fade_start(ledc_channel[ch].speed_mode, ledc_channel[ch].channel, LEDC_FADE_NO_WAIT);

    LED = NULL;
    vTaskDelete(LED);
}

void LEDHandler()
{
    if(LED == NULL)
    {
        printf("creating LED task\n");
        xTaskCreate(toggle_LED_task, "toggle_LED_task", 4096, NULL, 5, &LED);  
        configASSERT(LED);
    }
}

static void check_efuse(void)
{
    //Check TP is burned into eFuse
    if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_TP) == ESP_OK) {
        printf("eFuse Two Point: Supported\n");
    } else {
        printf("eFuse Two Point: NOT supported\n");
    }

    //Check Vref is burned into eFuse
    if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_VREF) == ESP_OK) {
        printf("eFuse Vref: Supported\n");
    } else {
        printf("eFuse Vref: NOT supported\n");
    }
}

static void print_char_val_type(esp_adc_cal_value_t val_type)
{
    if (val_type == ESP_ADC_CAL_VAL_EFUSE_TP) {
        printf("Characterized using Two Point Value\n");
    } else if (val_type == ESP_ADC_CAL_VAL_EFUSE_VREF) {
        printf("Characterized using eFuse Vref\n");
    } else {
        printf("Characterized using Default Vref\n");
    }
}

void run_button()
{
    LEDHandler();

    //Check if Two Point or Vref are burned into eFuse
    check_efuse();

    //Configure ADC
    if (unit == ADC_UNIT_1) {
        adc1_config_width(ADC_WIDTH_BIT_12);
        adc1_config_channel_atten(channel, atten);
    } else {
        adc2_config_channel_atten((adc2_channel_t)channel, atten);
    }

    //Characterize ADC
    adc_chars = calloc(1, sizeof(esp_adc_cal_characteristics_t));
    esp_adc_cal_value_t val_type = esp_adc_cal_characterize(unit, atten, ADC_WIDTH_BIT_12, DEFAULT_VREF, adc_chars);
    print_char_val_type(val_type);

    //Continuously sample ADC1
    while (1) {
        uint32_t adc_reading = 0;
        //Multisampling
        for (int i = 0; i < NO_OF_SAMPLES; i++) {
            if (unit == ADC_UNIT_1) {
                adc_reading += adc1_get_raw((adc1_channel_t)channel);
            } else {
                int raw;
                adc2_get_raw((adc2_channel_t)channel, ADC_WIDTH_BIT_12, &raw);
                adc_reading += raw;
            }
        }
        adc_reading /= NO_OF_SAMPLES;

        //Convert adc_reading to voltage in mV
        uint32_t voltage = esp_adc_cal_raw_to_voltage(adc_reading, adc_chars);
        if (adc_reading > 4000) {
            buttonPress = 1;
        }
        else {
            buttonPress = 0;
        }
        printf("button pressed: %d\n", buttonPress);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

#endif
