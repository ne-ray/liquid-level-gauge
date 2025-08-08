#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"

#define LED_PIN CONFIG_LED_GPIO

void led_set_state(bool power)
{
    /* Set the GPIO level according to the state (LOW or HIGH)*/
    gpio_set_level(LED_PIN, !power);
}

// --- Функция мигания светодиодом ---
void led_flash(int times, int delay_ms)
{
    for (int i = 0; i < times; i++)
    {
        led_set_state(true); // Включить
        vTaskDelay(pdMS_TO_TICKS(delay_ms));

        led_set_state(false); // Выключить
        vTaskDelay(pdMS_TO_TICKS(delay_ms));
    }
}

void init_led(void)
{
    // LED PIN
    gpio_reset_pin(LED_PIN);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);
}