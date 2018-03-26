#include <stdlib.h>
#include <stdio.h>

#include <webots/robot.h>

#include "../util/camera.h"
#include "../util/leds.h"
#include "../util/consts.h"

#define RED_LED     7
#define RED_H_MIN   0.85
#define RED_H_MAX   1
#define RED_L_MIN   0.1
#define RED_L_MAX   0.9
#define RED_S_MIN   0.2
#define RED_S_MAX   1

#define GREEN_LED   0
#define GREEN_H_MIN 0.25
#define GREEN_H_MAX 0.45
#define GREEN_L_MIN 0.1
#define GREEN_L_MAX 0.9
#define GREEN_S_MIN 0.3
#define GREEN_S_MAX 1

#define BLUE_LED    1
#define BLUE_H_MIN  0.45
#define BLUE_H_MAX  0.6
#define BLUE_L_MIN  0.1
#define BLUE_L_MAX  0.95
#define BLUE_S_MIN  0.3
#define BLUE_S_MAX  1

#define is_between(value, min, max) ((value) >= (min) && (value) <= (max))

#define get_max(a, b) (a) > (b) ? (a) : (b)
#define get_min(a, b) (a) < (b) ? (a) : (b)

typedef struct
{
    float h;
    float l;
    float s;
} hls_color;

hls_color to_hls(rgb_color input)
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
        float d = maxf - minf;
        
        if(light > 0.5)
            sat = d / (2 - maxf - minf);
        else
            sat = d / (maxf + minf);
        
        if(max == input.r)
            hue = (gf - bf) / d + (gf < bf ? 6 : 0);
        else if(max == input.g)
            hue = (bf - rf) / d + 2;
        else if(max == input.b)
            hue = (rf - gf) / d + 4;
        hue /= 6;
    }
    
    return (hls_color){hue, light, sat};
}

int main()
{
    wb_robot_init();
    leds_init();
    camera_init();
    
    while(wb_robot_step(TIME_STEP) != -1)
    {
        rgb_color average = camera_get_average_color();
        hls_color color = to_hls(average);
        
//        printf("h: %3d°\t\tl: %3d%%\t\ts: %3d%%\n",
//               (int) (color.h * 360), (int) (color.l * 100), (int) (color.s * 100));
        
        printf("h: %3f\t\tl: %3f\t\ts: %3f\n", color.h, color.l, color.s);
        
        leds_set(false);
        
        if(is_between(color.h, RED_H_MIN, RED_H_MAX) &&
           is_between(color.l, RED_L_MIN, RED_L_MAX) &&
           is_between(color.s, RED_S_MIN, RED_S_MAX))
            led_set(RED_LED, true);
        
        else if(is_between(color.h, GREEN_H_MIN, GREEN_H_MAX) &&
                is_between(color.l, GREEN_L_MIN, GREEN_L_MAX) &&
                is_between(color.s, GREEN_S_MIN, GREEN_S_MAX))
            led_set(GREEN_LED, true);
        
        else if(is_between(color.h, BLUE_H_MIN, BLUE_H_MAX) &&
                is_between(color.l, BLUE_L_MIN, BLUE_L_MAX) &&
                is_between(color.s, BLUE_S_MIN, BLUE_S_MAX))
            led_set(BLUE_LED, true);
    }
    
    wb_robot_cleanup();
    return EXIT_SUCCESS;
}
