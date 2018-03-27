#include <stdlib.h>

#include <webots/robot.h>
#include <stdio.h>

#include "../util/leds.h"
#include "../util/motors.h"
#include "../util/prox_sensors.h"
#include "../util/camera.h"
#include "../util/consts.h"

#define THRESHOLD 400
#define SPEED 2.5
#define MAX_SPEED 6

#define RED_LED     7
#define RED_H_MIN   0.85
#define RED_H_MAX   1
#define RED_S_MIN   0.2
#define RED_S_MAX   1
#define RED_L_MIN   0.1
#define RED_L_MAX   0.9

#define GREEN_LED   0
#define GREEN_H_MIN 0.25
#define GREEN_H_MAX 0.45
#define GREEN_S_MIN 0.3
#define GREEN_S_MAX 1
#define GREEN_L_MIN 0.1
#define GREEN_L_MAX 0.9

#define BLUE_LED    1
#define BLUE_H_MIN  0.45
#define BLUE_H_MAX  0.7
#define BLUE_S_MIN  0.3
#define BLUE_S_MAX  1
#define BLUE_L_MIN  0.1
#define BLUE_L_MAX  0.95

#define is_between(value, min, max) ((value) >= (min) && (value) <= (max))

#define get_max(a, b) (a) > (b) ? (a) : (b)
#define get_min(a, b) (a) < (b) ? (a) : (b)

typedef struct
{
    float h;
    float s;
    float l;
} hsl_color;

hsl_color to_hsl(rgb_color input)
{
    unsigned max = get_max(get_max(input.r, input.g), input.b);
    unsigned min = get_min(get_min(input.r, input.g), input.b);
    
    float maxf = (float) max / 255;
    float minf = (float) min / 255;
    
    float hue = 0;
    float sat = 0;
    float light = (maxf + minf) / 2;
    
    if(max != min)
    {
        float rf = (float) input.r / 255;
        float gf = (float) input.g / 255;
        float bf = (float) input.b / 255;
        float delta = maxf - minf;
        
        if(light > 0.5)
            sat = delta / (2 - maxf - minf);
        else
            sat = delta / (maxf + minf);
        
        if(max == input.r)
            hue = (gf - bf) / delta + (gf < bf ? 6 : 0);
        else if(max == input.g)
            hue = (bf - rf) / delta + 2;
        else if(max == input.b)
            hue = (rf - gf) / delta + 4;
        hue /= 6;
    }
    
    return (hsl_color) {hue, light, sat};
}

int main()
{
    wb_robot_init();
    motors_init();
    leds_init();
    prox_init();
    camera_init();
    
    static const double total_weight = 10;
    static const double weights[PROX_COUNT] =
            {4, 3, 2, 1, 1, 2, 3, 4};
    
    while(wb_robot_step(TIME_STEP) != -1)
    {
        // compute speed
        
        double prox[2] = {0, 0};
        for(size_t i = 0; i < PROX_COUNT; ++i)
        {
            size_t index = i * 2 / PROX_COUNT;
            prox[index] += weights[i] * prox_get_value(i, true) / total_weight;
        }
        
        double speed_left = SPEED - prox[0] / THRESHOLD * SPEED;
        double speed_right = SPEED - prox[1] / THRESHOLD * SPEED;
        
        motors_set_speed(speed_left, speed_right);
        
        
        // check color
        
        rgb_color average = camera_get_average_color();
        hsl_color color = to_hsl(average);
        
        printf("h: %3d°\t\tl: %3d%%\t\ts: %3d%%\n",
               (int) (color.h * 360), (int) (color.l * 100), (int) (color.s * 100));
        
        leds_set(false);
        
        if(is_between(color.h, RED_H_MIN, RED_H_MAX) &&
           is_between(color.s, RED_S_MIN, RED_S_MAX) &&
           is_between(color.l, RED_L_MIN, RED_L_MAX))
            led_set(RED_LED, true);
        
        else if(is_between(color.h, GREEN_H_MIN, GREEN_H_MAX) &&
                is_between(color.s, GREEN_S_MIN, GREEN_S_MAX) &&
                is_between(color.l, GREEN_L_MIN, GREEN_L_MAX))
            led_set(GREEN_LED, true);
        
        else if(is_between(color.h, BLUE_H_MIN, BLUE_H_MAX) &&
                is_between(color.s, BLUE_S_MIN, BLUE_S_MAX) &&
                is_between(color.l, BLUE_L_MIN, BLUE_L_MAX))
            led_set(BLUE_LED, true);
    }
    
    wb_robot_cleanup();
    return EXIT_SUCCESS;
}
