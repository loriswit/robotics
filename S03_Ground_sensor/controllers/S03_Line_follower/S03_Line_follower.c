#include <stdlib.h>

#include <webots/robot.h>

#include "../util/motors.h"
#include "../util/ground_sensors.h"
#include "../util/consts.h"

#define GROUND_MAX 1000
#define THRESHOLD 250
#define SPEED 1

double array_sum(const double array[], size_t length)
{
    double sum = 0;
    for(size_t i = 0; i < length; ++i)
        sum += array[i];
    
    return sum;
}

int main()
{
    wb_robot_init();
    motors_init();
    ground_init();
    
    static const double right_weights[GROUND_COUNT] = {0, 0, 1};
    static const double left_weights[GROUND_COUNT] = {1, 0, 0};
    
    while(wb_robot_step(TIME_STEP) != -1)
    {
        double ground_right = GROUND_MAX - ground_get_value(GROUND_RIGHT);
        double ground_center = GROUND_MAX - ground_get_value(GROUND_CENTER);
        double ground_left = GROUND_MAX - ground_get_value(GROUND_LEFT);
        
        double right_factor =
                ground_right * right_weights[GROUND_RIGHT] +
                ground_center * right_weights[GROUND_CENTER] +
                ground_left * right_weights[GROUND_LEFT];
        double left_factor =
                ground_right * left_weights[GROUND_RIGHT] +
                ground_center * left_weights[GROUND_CENTER] +
                ground_left * left_weights[GROUND_LEFT];
        
        right_factor /= array_sum(right_weights, GROUND_COUNT);
        left_factor /= array_sum(left_weights, GROUND_COUNT);
    
        double speed_right = SPEED + right_factor / THRESHOLD * SPEED;
        double speed_left = SPEED + left_factor / THRESHOLD * SPEED;
        
        motors_set_speed(speed_right, speed_left);
    }
    
    wb_robot_cleanup();
    return EXIT_SUCCESS;
}
