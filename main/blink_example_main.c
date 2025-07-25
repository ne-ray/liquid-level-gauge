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

    // PIN 1 SET
    gpio_reset_pin(CONTACT_PIN_1);
    gpio_set_direction(CONTACT_PIN_1, GPIO_MODE_INPUT);  // Установить как вход
    gpio_set_pull_mode(CONTACT_PIN_1, GPIO_PULLUP_ONLY); // Включить внутреннюю подтяжку вверх

    // PIN 2 SET
    gpio_reset_pin(CONTACT_PIN_2);
    gpio_set_direction(CONTACT_PIN_2, GPIO_MODE_INPUT);  // Установить как вход
    gpio_set_pull_mode(CONTACT_PIN_2, GPIO_PULLUP_ONLY); // Включить внутреннюю подтяжку вверх
}

void app_main(void)
{

    /* Configure the peripheral according to the LED type */
    configure_pin();

    while (1)
    {
        // 3. Чтение состояния входного контакта
        int contact_state1 = gpio_get_level(CONTACT_PIN_1);
        int contact_state2 = gpio_get_level(CONTACT_PIN_2);

        uint8_t new_state = 0;

        // 4. Логика управления светодиодом
        if (contact_state1 == 0 || contact_state2 == 0)
        {                  // Если контакт замкнут (уровень LOW)
            new_state = 0; // Включить светодиод (HIGH)
        }
        else
        {                  // Если контакт разомкнут (уровень HIGH)
            new_state = 1; // Выключить светодиод (LOW)
        }

        if (new_state != s_led_state)
        {
            s_led_state = new_state;

            if (new_state == 0)
            {
                ESP_LOGI(TAG, "Contact open! LED ON.");
            }
            else
            {
                ESP_LOGI(TAG, "Contact close! LED OFF.");
            }
        }

        blink_led();
        /* Toggle the LED state */
        // s_led_state = !s_led_state;
        vTaskDelay(CONFIG_CHECK_PERIOD / portTICK_PERIOD_MS);
    }
}
