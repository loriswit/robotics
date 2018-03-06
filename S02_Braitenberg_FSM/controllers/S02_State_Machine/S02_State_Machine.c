#include <stdlib.h>
#include <stdio.h>

#include <webots/robot.h>

#include "../util/motors.h"
#include "../util/leds.h"
#include "../util/sensors.h"
#include "../util/consts.h"

#define THRESHOLD 400
#define SPEED 4
#define MAX_SPEED 6
#define LOVER_DISTANCE_THRESHOLD 1700
#define EXPLORER_DISTANCE_THRESHOLD 50
#define MIN_STATE_DURATION 2.5 // in seconds

double clamp(double value, double min, double max)
{
    if(value > max)
        return max;
    
    if(value < min)
        return min;
    
    return value;
}

int main()
{
    wb_robot_init();
    motors_init();
    leds_init();
    sensors_init();
    
    static const double total_weight = 10;
    static const double lover_weights[SENSORS_COUNT] =
            {2, 2, 3, 4, 4, 3, 2, 2};
    static const double explorer_weights[SENSORS_COUNT] =
            {4, 3, 2, 1, 1, 2, 3, 4};
    
    const double time_factor = wb_robot_get_mode() == 0 ? 1 : 0.4;
    printf("time factor = %f\n", time_factor);
    
    const unsigned max_counter = MIN_STATE_DURATION * 1000 / TIME_STEP * time_factor;
    unsigned counter = max_counter;
    
    enum { LOVER, EXPLORER } state = LOVER;
    
    while(wb_robot_step(TIME_STEP) != -1)
    {
        double distance =
                (sensors_get_value(0, true) + sensors_get_value(7, true)) / 2;
        
        if(++counter >= max_counter)
        {
            if(state == LOVER && distance > LOVER_DISTANCE_THRESHOLD)
            {
                state = EXPLORER;
                counter = 0;
                printf("switching to explorer\n");
            }
            
            else if(state == EXPLORER && distance < EXPLORER_DISTANCE_THRESHOLD)
            {
                state = LOVER;
                counter = max_counter * 3 / 4;
                printf("switching to lover\n");
            }
        }
        
        double prox[2] = {0, 0};
        for(size_t i = 0; i < SENSORS_COUNT; ++i)
        {
            double weight = state == LOVER ? lover_weights[i] : explorer_weights[i];
            prox[i * 2 / SENSORS_COUNT] +=
                    weight * sensors_get_value(i, true) / total_weight;
        }
        
        double speed_right = SPEED - prox[0] / THRESHOLD * SPEED;
        double speed_left = SPEED - prox[1] / THRESHOLD * SPEED;
        
        motors_set_speed(
                clamp(state == LOVER ? speed_left : speed_right, -MAX_SPEED, MAX_SPEED),
                clamp(state == LOVER ? speed_right : speed_left, -MAX_SPEED, MAX_SPEED));
        
        leds_set(state == LOVER);
    }
    
    leds_set(false);
    
    wb_robot_cleanup();
    return EXIT_SUCCESS;
}
