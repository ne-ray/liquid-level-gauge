#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_log.h"
#include "esp_sleep.h"
#include "sdkconfig.h"

static const char *TAG = "example";

/* Use project configuration menu (idf.py menuconfig) to choose the GPIO to blink,
   or you can edit the following line and set a number here.
*/
#define LED_PIN CONFIG_LED_GPIO
#define CLOSURE_PIN_1 CONFIG_CLOSURE_PIN_1
#define CLOSURE_PIN_2 CONFIG_CLOSURE_PIN_2
#define POTENTIOMETER_PIN_1 CONFIG_POTENTIOMETER_PIN_1
#define SLEEP_WAKEUP_TIMER CONFIG_ESP_SLEEP_WAKEUP_TIMER

#define ADC_UNIT ADC_UNIT_1           // Используем ADC1
#define ADC_CHANNEL_POT ADC_CHANNEL_2 // Соответствует GPIO2 для ADC1

static uint8_t led_state = 0;
static uint8_t closure_1_state = 0;
static uint8_t closure_2_state = 0;
static adc_oneshot_unit_handle_t adc1_handle;
static int potentiometer_raw_value = 0;

void led_set_state(int new_state)
{
    /* Set the GPIO level according to the state (LOW or HIGH)*/
    gpio_set_level(LED_PIN, new_state);
}

// --- Функция мигания светодиодом ---
void led_flash(int times, int delay_ms)
{
    for (int i = 0; i < times; i++)
    {
        led_set_state(0); // Включить
        vTaskDelay(pdMS_TO_TICKS(delay_ms));
        led_set_state(1); // Выключить
        vTaskDelay(pdMS_TO_TICKS(delay_ms));
    }
}

static void led_init(void)
{
    // LED PIN
    ESP_LOGI(TAG, "Configured to GPIO LED!");
    gpio_reset_pin(LED_PIN);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);
}

static void init_clouser_pins(void)
{
    gpio_reset_pin(CLOSURE_PIN_1);
    gpio_set_direction(CLOSURE_PIN_1, GPIO_MODE_INPUT);
    gpio_set_pull_mode(CLOSURE_PIN_1, GPIO_PULLUP_ONLY); // Включить внутреннюю подтяжку вверх для нормальной работы

    gpio_reset_pin(CLOSURE_PIN_2);
    gpio_set_direction(CLOSURE_PIN_2, GPIO_MODE_INPUT);
    gpio_set_pull_mode(CLOSURE_PIN_2, GPIO_PULLUP_ONLY); // Включить внутреннюю подтяжку вверх
}

static void init_potentiometer(void)
{
    // --- 1. Инициализация единицы АЦП ---
    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = ADC_UNIT, // Выбираем ADC1
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config, &adc1_handle));
    ESP_LOGI(TAG, "Единица АЦП (ADC%d) инициализирована.", ADC_UNIT + 1);

    // --- 2. Конфигурация канала АЦП ---
    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_12, // Разрешение 12 бит (0-4095)
        .atten = ADC_ATTEN_DB_11,    // Ослабление 11 дБ для измерения до ~3.3V
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, ADC_CHANNEL_POT, &config));
    ESP_LOGI(TAG, "Канал АЦП (GPIO%d) сконфигурирован.", POTENTIOMETER_PIN_1);
}

static void get_clouser_pin1(void)
{
    // 3. Чтение состояния входного контакта
    closure_1_state = gpio_get_level(CLOSURE_PIN_1);

    ESP_LOGI(TAG, "Состояние контакта 1: %s", closure_1_state ? "Не замкнут" : "Замкнут");
}

static void get_clouser_pin2(void)
{
    // 3. Чтение состояния входного контакта
    closure_2_state = gpio_get_level(CLOSURE_PIN_2);

    ESP_LOGI(TAG, "Состояние контакта 2: %s", closure_2_state ? "Не замкнут" : "Замкнут");
}

static void get_potentiometer(void)
{
    ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, ADC_CHANNEL_POT, &potentiometer_raw_value));
    ESP_LOGI(TAG, "Сырое значение АЦП: %d", potentiometer_raw_value);
}

static void next_wakeup_config(void)
{
    // Настройка пинов кнопок для пробуждения по GPIO
    // 3.1. Кнопка 1 (GPIO0)
    // init_pin1();

    // Если контакт замкнут - будем просыпаться по таймауту, иначе будем ожидать замыкания
    if (closure_1_state == 0)
    {
        gpio_pullup_en(CLOSURE_PIN_1);    // Включить подтяжку вверх для режима глубокого сна
        gpio_pulldown_dis(CLOSURE_PIN_1); // Отключить подтяжку вниз
    }
    else
    {
        gpio_pullup_dis(CLOSURE_PIN_1);   // Отключить подтяжку вверх
        gpio_pulldown_dis(CLOSURE_PIN_1); // Отключить подтяжку вниз
    }

    // 3.2. Кнопка 2 (GPIO1)
    // init_pin2();
    gpio_pullup_en(CLOSURE_PIN_2);    // Включить подтяжку вверх для режима глубокого сна
    gpio_pulldown_dis(CLOSURE_PIN_2); // Отключить подтяжку вниз

    // Включаем пробуждение по GPIO для обеих кнопок при низком уровне
    // Создаем маску, объединяющую биты для каждого пина
    uint64_t gpio_wakeup_mask = (1ULL << CLOSURE_PIN_1) | (1ULL << CLOSURE_PIN_2);
    esp_deep_sleep_enable_gpio_wakeup(gpio_wakeup_mask, ESP_GPIO_WAKEUP_GPIO_LOW);
    ESP_LOGI(TAG, "Настройка пробуждения по GPIO (Кнопка 1: GPIO%d, Кнопка 2: GPIO%d).", CLOSURE_PIN_1, CLOSURE_PIN_2);

    // 3.3. Настройка пробуждения по таймеру
    // esp_deep_sleep_enable_timer_wakeup(SLEEP_WAKEUP_TIMER * 1000);
    // ESP_LOGI(TAG, "Настройка пробуждения по таймеру каждые %d секунд.", SLEEP_WAKEUP_TIMER / 1000);
}

void app_main(void)
{
    // 1. Инициализация светодиода (делается при каждом пробуждении)
    led_init();
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
        get_clouser_pin1();
        get_clouser_pin2();
        get_potentiometer();
        vTaskDelay(SLEEP_WAKEUP_TIMER / 10 / portTICK_PERIOD_MS);
    }

    // --- 3. Настройка источников пробуждения для СЛЕДУЮЩЕГО глубокого сна ---
    next_wakeup_config();

    // --- 4. Вход в глубокий сон ---
    ESP_LOGI(TAG, "Вход в глубокий сон...");
    esp_deep_sleep(SLEEP_WAKEUP_TIMER * 1000);
    // esp_deep_sleep_start();
}
