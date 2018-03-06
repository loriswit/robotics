#include <stdlib.h>

#include <webots/robot.h>

#include "../util/motors.h"
#include "../util/leds.h"
#include "../util/sensors.h"
#include "../util/consts.h"

#define THRESHOLD 400
#define SPEED 4
#define MAX_SPEED 6

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
    static const double weights[SENSORS_COUNT] =
            {2, 2, 3, 4, 4, 3, 2, 2};
    
    while(wb_robot_step(TIME_STEP) != -1)
    {
        double prox[2] = {0, 0};
        for(size_t i = 0; i < SENSORS_COUNT; ++i)
            prox[i * 2 / SENSORS_COUNT] +=
                    weights[i] * sensors_get_value(i, true) / total_weight;
        
        double speed_right = SPEED - prox[0] / THRESHOLD * SPEED;
        double speed_left = SPEED - prox[1] / THRESHOLD * SPEED;
        
        motors_set_speed(
                clamp(speed_left, -MAX_SPEED, MAX_SPEED),
                clamp(speed_right, -MAX_SPEED, MAX_SPEED));
    }
    
    wb_robot_cleanup();
    return EXIT_SUCCESS;
}
