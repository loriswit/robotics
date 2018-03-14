#ifndef ROBOTICS_LEDS_H
#define ROBOTICS_LEDS_H

#include <stdbool.h>

#define LEDS_COUNT 8

/**
 * Initializes all LEDs.
 */
void leds_init();

/**
 * Turns all LEDs on or off.
 *
 * @param value TRUE to turn LEDs on, FALSE to turn off
 */
void leds_set(bool value);

/**
 * Turns all LEDs on or off according to the current step
 */
void leds_flash();

#endif //ROBOTICS_LEDS_H
