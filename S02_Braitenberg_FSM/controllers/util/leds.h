#ifndef S02_BRAITENBERG_FSM_LED_HPP
#define S02_BRAITENBERG_FSM_LED_HPP

#define LEDS_COUNT 8

// initializes LEDs
void leds_init();

// turns all LEDs on or off
void leds_set(bool value);

// turns all LEDs on or off according to the current step
void leds_flash();

#endif //S02_BRAITENBERG_FSM_LED_HPP
