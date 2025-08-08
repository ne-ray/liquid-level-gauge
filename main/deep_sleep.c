#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_sleep.h"
#include "level_gauge.h"
#include "sdkconfig.h"

void config_next_wakeup_level_gauge(void)
{
    // Настройка пинов кнопок для пробуждения по GPIO
    // 3.1. Кнопка 1 (GPIO0)
    // init_pin1();

    // Если контакт замкнут - будем просыпаться по таймауту, иначе будем ожидать замыкания
    if (get_clouser_pin1() == 0)
    {
        // gpio_pullup_en(CLOSURE_PIN_1);    // Включить подтяжку вверх для режима глубокого сна
        // gpio_pulldown_dis(CLOSURE_PIN_1); // Отключить подтяжку вниз
    }
    else
    {
        // gpio_pullup_dis(CLOSURE_PIN_1);   // Отключить подтяжку вверх
        // gpio_pulldown_dis(CLOSURE_PIN_1); // Отключить подтяжку вниз
    }

    // 3.2. Кнопка 2 (GPIO1)
    // init_pin2();
    // gpio_pullup_en(CLOSURE_PIN_2);    // Включить подтяжку вверх для режима глубокого сна
    // gpio_pulldown_dis(CLOSURE_PIN_2); // Отключить подтяжку вниз

    // Включаем пробуждение по GPIO для обеих кнопок при низком уровне
    // Создаем маску, объединяющую биты для каждого пина
    uint64_t gpio_wakeup_mask = (1ULL << CLOSURE_PIN_1) | (1ULL << CLOSURE_PIN_2);
    esp_deep_sleep_enable_gpio_wakeup(gpio_wakeup_mask, ESP_GPIO_WAKEUP_GPIO_LOW);
    // ESP_LOGI(TAG, "Настройка пробуждения по GPIO (Кнопка 1: GPIO%d, Кнопка 2: GPIO%d).", CLOSURE_PIN_1, CLOSURE_PIN_2);

    // 3.3. Настройка пробуждения по таймеру
    // esp_deep_sleep_enable_timer_wakeup(SLEEP_WAKEUP_TIMER * 1000);
    // ESP_LOGI(TAG, "Настройка пробуждения по таймеру каждые %d секунд.", SLEEP_WAKEUP_TIMER / 1000);
}