#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_log.h"
#include "sdkconfig.h"

/* Use project configuration menu (idf.py menuconfig) to choose the GPIO to blink,
   or you can edit the following line and set a number here.
*/
#define CLOSURE_PIN_1 CONFIG_CLOSURE_PIN_1
#define CLOSURE_PIN_2 CONFIG_CLOSURE_PIN_2
#define POTENTIOMETER_PIN_1 CONFIG_POTENTIOMETER_PIN_1
#define SLEEP_WAKEUP_TIMER CONFIG_ESP_SLEEP_WAKEUP_TIMER

#define ADC_UNIT ADC_UNIT_1           // Используем ADC1
#define ADC_CHANNEL_POT ADC_CHANNEL_2 // Соответствует GPIO2 для ADC1

static adc_oneshot_unit_handle_t adc1_handle;

void init_clouser_pins(void)
{
    gpio_reset_pin(CLOSURE_PIN_1);
    gpio_set_direction(CLOSURE_PIN_1, GPIO_MODE_INPUT);
    gpio_set_pull_mode(CLOSURE_PIN_1, GPIO_PULLUP_ONLY); // Включить внутреннюю подтяжку вверх для нормальной работы

    gpio_reset_pin(CLOSURE_PIN_2);
    gpio_set_direction(CLOSURE_PIN_2, GPIO_MODE_INPUT);
    gpio_set_pull_mode(CLOSURE_PIN_2, GPIO_PULLUP_ONLY); // Включить внутреннюю подтяжку вверх
}

void init_potentiometer(void)
{
    // --- 1. Инициализация единицы АЦП ---
    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = ADC_UNIT, // Выбираем ADC1
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config, &adc1_handle));
    // ESP_LOGI(TAG, "Единица АЦП (ADC%d) инициализирована.", ADC_UNIT + 1);

    // --- 2. Конфигурация канала АЦП ---
    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_12, // Разрешение 12 бит (0-4095)
        .atten = ADC_ATTEN_DB_12,    // Ослабление 11 дБ для измерения до ~3.3V
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, ADC_CHANNEL_POT, &config));
    // ESP_LOGI(TAG, "Канал АЦП (GPIO%d) сконфигурирован.", POTENTIOMETER_PIN_1);
}

int get_clouser_pin1(void)
{
    // 3. Чтение состояния входного контакта
    // 1 - замкнут, 0 - не замкнут
    return !gpio_get_level(CLOSURE_PIN_1);
}

int get_clouser_pin2(void)
{
    // 3. Чтение состояния входного контакта
    // 1 - замкнут, 0 - не замкнут
    return !gpio_get_level(CLOSURE_PIN_2);
}

int get_potentiometer(void)
{
    static int potentiometer_raw_value = 0;
    ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, ADC_CHANNEL_POT, &potentiometer_raw_value));

    return potentiometer_raw_value;
}