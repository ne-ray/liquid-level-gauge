/* Blink Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "sdkconfig.h"

static const char *TAG = "example";

/* Use project configuration menu (idf.py menuconfig) to choose the GPIO to blink,
   or you can edit the following line and set a number here.
*/
#define LED_PIN CONFIG_LED_GPIO
#define CONTACT_PIN_1 CONFIG_CONTACT_PIN_GPIO_1
#define CONTACT_PIN_2 CONFIG_CONTACT_PIN_GPIO_2

static uint8_t s_led_state = 0;

static void blink_led(void)
{
    /* Set the GPIO level according to the state (LOW or HIGH)*/
    gpio_set_level(LED_PIN, s_led_state);
}

static void configure_pin(void)
{
    ESP_LOGI(TAG, "Example configured to blink GPIO LED!");
    gpio_reset_pin(LED_PIN);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);
}

void app_main(void)
{

    /* Configure the peripheral according to the LED type */
    configure_pin();

    while (1) {
        ESP_LOGI(TAG, "Turning the LED %s!", s_led_state == true ? "OFF" : "ON");
        blink_led();
        /* Toggle the LED state */
        s_led_state = !s_led_state;
        vTaskDelay(CONFIG_CHECK_PERIOD / portTICK_PERIOD_MS);
    }
}
