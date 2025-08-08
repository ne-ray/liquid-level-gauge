#include <stdbool.h>
#include "sdkconfig.h"

#define CLOSURE_PIN_1 CONFIG_CLOSURE_PIN_1
#define CLOSURE_PIN_2 CONFIG_CLOSURE_PIN_2
#define POTENTIOMETER_PIN_1 CONFIG_POTENTIOMETER_PIN_1
#define SLEEP_WAKEUP_TIMER CONFIG_ESP_SLEEP_WAKEUP_TIMER

#define ADC_UNIT ADC_UNIT_1           // Используем ADC1
#define ADC_CHANNEL_POT ADC_CHANNEL_2 // Соответствует GPIO2 для ADC1

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
