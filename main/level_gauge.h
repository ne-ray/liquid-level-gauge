#include <stdbool.h>
#include "sdkconfig.h"

#define LED_PIN CONFIG_LED_GPIO
#define CLOSURE_PIN_1 CONFIG_CLOSURE_PIN_1
#define CLOSURE_PIN_2 CONFIG_CLOSURE_PIN_2
#define POTENTIOMETER_PIN_1 CONFIG_POTENTIOMETER_PIN_1
#define SLEEP_WAKEUP_TIMER CONFIG_ESP_SLEEP_WAKEUP_TIMER

#define ADC_UNIT ADC_UNIT_1           // Используем ADC1
#define ADC_CHANNEL_POT ADC_CHANNEL_2 // Соответствует GPIO2 для ADC1

/**
 * @brief Set light power (on/off).
 *
 * @param  power  The light power to be set
 */
void led_set_state(bool power);

/*** @brief Flash the LED a specified number of times with a delay.
 *
 * @param  times      The number of times to flash the LED
 * @param  delay_ms   The delay in milliseconds between flashes
 */
void led_flash(int times, int delay_ms);

/**
 * @brief Initialize the LED GPIO.
 */
void init_led(void);

/**
 * @brief Initialize the closure pins.
 */
void init_clouser_pins(void);

/**
 * @brief Initialize the potentiometer ADC channel.
 */
void init_potentiometer(void);

/**
 * @brief Read the state of closure pin 1.
 */
int get_clouser_pin1(void);

/**
 * @brief Read the state of closure pin 2.
 */
int get_clouser_pin2(void);

/**
 * @brief Read the raw value from the potentiometer.
 */
int get_potentiometer(void);

/**
 * @brief Configure the next wakeup settings for the level gauge.
 */
void config_next_wakeup_level_gauge(void);