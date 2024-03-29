#include <webots/robot.h>
#include <webots/led.h>

#include <stdlib.h>

#include "leds.h"

WbDeviceTag leds[LEDS_COUNT];

void leds_init()
{
    static const char * led_names[LEDS_COUNT] =
            {"led0", "led1", "led2", "led3", "led4", "led5", "led6", "led7"};
    
    for(size_t i = 0; i < LEDS_COUNT; ++i)
        leds[i] = wb_robot_get_device(led_names[i]);
}

void leds_set(bool value)
{
    for(size_t i = 0; i < LEDS_COUNT; ++i)
        wb_led_set(leds[i], value);
}

void leds_flash()
{
    static unsigned step = 0;
    leds_set((step++ / 2) % 2 == 0);
}
