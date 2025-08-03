#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_sleep.h"
#include "nvs_flash.h"
#include "main.h"

static const char *TAG = "liquid_level_gauge";

static void next_wakeup_config(void)
{
    config_next_wakeup_level_gauge();
}

void app_main(void)
{
    // 1. Инициализация светодиода (делается при каждом пробуждении)
    ESP_LOGI(TAG, "Configured to GPIO LED!");
    init_led();

    init_clouser_pins();
    init_potentiometer();
    get_clouser_pin1();
    get_clouser_pin2();

    // 2. Проверка причины пробуждения
    esp_sleep_wakeup_cause_t wakeup_cause = esp_sleep_get_wakeup_cause();
    switch (wakeup_cause)
    {
        case ESP_SLEEP_WAKEUP_GPIO:
        {
            ESP_LOGI(TAG, "Пробуждение по GPIO!");
            uint64_t gpio_wakeup_status = esp_sleep_get_gpio_wakeup_status();

            if (gpio_wakeup_status & (1ULL << CLOSURE_PIN_1))
            {
                ESP_LOGI(TAG, "Пробуждение по Кнопке 1 (GPIO%d).", CLOSURE_PIN_1);
                led_flash(1, 200); // Мигнуть 1 раз
            }
            if (gpio_wakeup_status & (1ULL << CLOSURE_PIN_2))
            {
                ESP_LOGI(TAG, "Пробуждение по Кнопке 2 (GPIO%d).", CLOSURE_PIN_2);
                led_flash(2, 200); // Мигнуть 2 раза
            }
            break;
        }
        case ESP_SLEEP_WAKEUP_TIMER:
        {
            ESP_LOGI(TAG, "Пробуждение по таймеру.");
            get_potentiometer();
            
            break;
        }
        case ESP_SLEEP_WAKEUP_UNDEFINED:
        {
            ESP_LOGI(TAG, "Первая загрузка или пробуждение по сбросу питания.");
            break;
        }
        default:
        {
            ESP_LOGI(TAG, "Пробуждение по другой причине: %d.", wakeup_cause);
            break;
        }
    }

    // TMP test
    while (1)
    {
        int closure_1_state = get_clouser_pin1();
        ESP_LOGI(TAG, "Состояние контакта 1: %s", closure_1_state ? "Не замкнут" : "Замкнут");

        int closure_2_state = get_clouser_pin2();
        ESP_LOGI(TAG, "Состояние контакта 1: %s", closure_1_state ? "Не замкнут" : "Замкнут");

        int potentiometer_raw_value = get_potentiometer();
        ESP_LOGI(TAG, "Сырое значение АЦП: %d", potentiometer_raw_value);

        vTaskDelay(SLEEP_WAKEUP_TIMER / portTICK_PERIOD_MS);
    }

    // --- 3. Настройка источников пробуждения для СЛЕДУЮЩЕГО глубокого сна ---
    next_wakeup_config();

    // --- 4. Вход в глубокий сон ---
    ESP_LOGI(TAG, "Вход в глубокий сон...");
    esp_deep_sleep(SLEEP_WAKEUP_TIMER * 1000);
    // esp_deep_sleep_start();
}