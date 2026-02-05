/*********************************************************************************\
*
* MMMMMMMMMMMM   SSSSSSSSSSSS   WW   WW   WW   MECHATRONIK
* MM   MM   MM   SS             WW   WW   WW   SCHULE
* MM   MM   MM   SSSSSSSSSSSS   WW   WW   WW   WINTERTHUR
* MM   MM   MM             SS   WW   WW   WW   
* MM   MM   MM   SSSSSSSSSSSS   WWWWWWWWWWWW   www.msw.ch
*
*
* Dateiname: main.c
*
* Projekt  : ESP32 Board Treiber 
* Hardware : ESP32-S3
*
* Copyright: MSW, E4
*
* Beschreibung:
* =============
* Treiber für das ESP32 Board
*
* Portbelegung:
* =============
* Siehe Hardwarestruktur
*
* Verlauf:
* ========
* Datum:      Autor:         Version   Grund der Änderung:
* 23.08.2024  S. Huruvarshan V1.0      Neuerstellung
*
\*********************************************************************************/
#include <ESP-Driver.h>
#include <led_strip.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"


#define OUTPUT_WS2812 38 

#define INPUT_BTN0 21 
#define INPUT_BTN1 35 
#define INPUT_BTN2 36 
#define INPUT_BTN3 37

void blink_led_task(void *pvParameters);


void app_main(void)
{
    static TaskHandle_t blinkTaskHandle = NULL;
    static const char* TAG = "app_main"; // Ein Name für dein Modul
    //--------------------------------- LED Strip Configuration ---------------------------------
    led_strip_handle_t led_strip;

    /* LED strip initialization with the GPIO and pixels number*/
    led_strip_config_t strip_config = {
        .strip_gpio_num = OUTPUT_WS2812,            // The GPIO that connected to the LED strip's data line
        .max_leds = 2,                              // The number of LEDs in the strip,
        .led_pixel_format = LED_PIXEL_FORMAT_GRB,   // Pixel format of your LED strip
        .led_model = LED_MODEL_WS2812,              // LED strip model
        .flags.invert_out = false,                  // whether to invert the output signal (useful when your hardware has a level inverter)
    };

    led_strip_rmt_config_t rmt_config = {
    #if ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(5, 0, 0)
        .rmt_channel = 0,
    #else
        .clk_src = RMT_CLK_SRC_DEFAULT,             // different clock source can lead to different power consumption
        .resolution_hz = 10 * 1000 * 1000,          // 10MHz
        .flags.with_dma = false,                    // whether to enable the DMA feature
    #endif
    };
    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip)); 
    

    //--------------------------------- initialize board ---------------------------------
    initBoard(0);
    // Task erstellen: Name, Stack-Größe (4096), Parameter (NULL), Priorität (5), Handle
    //xTaskCreate(blink_led_task, "BlinkTask", 4096, NULL, 5, &blinkTaskHandle);
    uint32_t iterations = 0;


    while (1)
    {
        if (gpio_get_level(INPUT_BTN0) | gpio_get_level(INPUT_BTN1) | gpio_get_level(INPUT_BTN2) | gpio_get_level(INPUT_BTN3))
        {
            led_strip_set_pixel(led_strip, 0, 1, 25, 1); 
            led_strip_set_pixel(led_strip, 1, 1, 25, 1); 
            led_strip_refresh(led_strip);
        } else {
            led_strip_set_pixel(led_strip, 0, 25, 1, 1); 
            led_strip_set_pixel(led_strip, 1, 25, 1, 1); 
            led_strip_refresh(led_strip);
        }
        if(swtichReadAll()){
            if (blinkTaskHandle == NULL) {
                xTaskCreate(blink_led_task, "BlinkTask", 4096, NULL, 5, &blinkTaskHandle);
                ESP_LOGI("TASK", "Task wurde erstellt.");
            }
        }else{
            if (blinkTaskHandle != NULL) {
                vTaskDelete(blinkTaskHandle);
                blinkTaskHandle = NULL; // Wichtig, damit wir nicht auf ein ungültiges Handle zugreifen
                ledWriteAll(0);
                ESP_LOGI("TASK", "Task wurde gelöscht.");
            }
        }

        int64_t time_since_boot = esp_timer_get_time();//systime in us
        //ESP_LOGI("TIME", "Ich laufe seit %lld Sekunden", time_since_boot/1000/1000);
        //ledWriteAll(swtichReadAll()<<0 | buttonReadAll()<<8);
        //ledWriteAll(0x5555);

        //vTaskDelay(pdMS_TO_TICKS(10));//Warte 10 ms (auch Tasks mit tieferer Prio werden behandelt)
        //vTaskDelay(1);//Warte 1 Tick (10 ms, auch Tasks mit tieferer Prio werden behandelt)
        //vTaskDelay(0);//Warte 0 Ticks (0 ms, nur Tasks mit höherer Prio werden behandelt) 
        //taskYIELD();//Behandle Tasks mit höherer Prio
        //esp_task_wdt_reset(); // Den Watchdog für die aktuelle Task zurücksetzen
        //ESP_LOGI(TAG, "Das war Iteration %lu.",iterations);
        iterations++;
    }
}



// Die eigentliche Blink-Funktion
void blink_led_task(void *pvParameters) {
    ESP_LOGI("BLINK_TASK", "Blink-Task gestartet.");
    
    while(1) {
        
            ledWriteAll(0xFFFF); // Alle LEDs an
            vTaskDelay(pdMS_TO_TICKS(500));
            ledWriteAll(0x0000); // Alle LEDs aus
            vTaskDelay(pdMS_TO_TICKS(500));
        
        
            
        }
    
}