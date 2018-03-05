#include <stdlib.h>

#include <webots/robot.h>

#include "../util/motors.h"
#include "../util/leds.h"
#include "../util/sensors.h"
#include "../util/consts.h"

#define THRESHOLD 400
#define SPEED 2

int main()
{
    wb_robot_init();
    motors_init();
    leds_init();
    sensors_init();
    
    while(wb_robot_step(TIME_STEP) != -1)
    {
        double prox[2];
        for(size_t i = 0; i < SENSORS_COUNT; ++i)
            prox[i / SENSORS_COUNT * 2] += sensors_get_distance(i, true) / SENSORS_COUNT * 2;
        
        double speed_right = prox[0] / THRESHOLD * SPEED;
        double speed_left = prox[1] / THRESHOLD * SPEED;
        
        motors_set_speed(speed_left, speed_right);
    }
    
    wb_robot_cleanup();
    return EXIT_SUCCESS;
}
