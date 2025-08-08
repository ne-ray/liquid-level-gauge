#include <stdbool.h>
#include "level_gauge.h"
#include "sdkconfig.h"

#define SLEEP_WAKEUP_TIMER CONFIG_ESP_SLEEP_WAKEUP_TIMER

/**
 * @brief Configure the next wakeup settings for the level gauge.
 */
void config_next_wakeup_level_gauge(void);