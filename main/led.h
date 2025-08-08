#include <stdbool.h>
#include "sdkconfig.h"

#define LED_PIN CONFIG_LED_GPIO

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