#include <webots/robot.h>
#include <webots/motor.h>

#include "motors.h"

WbDeviceTag left_motor;
WbDeviceTag right_motor;

void motors_set_speed(double left, double right)
{
    wb_motor_set_velocity(left_motor, left);
    wb_motor_set_velocity(right_motor, right);
}

void motors_stop()
{
    motors_set_speed(0, 0);
}

void motors_init()
{
    left_motor = wb_robot_get_device("left wheel motor");
    right_motor = wb_robot_get_device("right wheel motor");
    
    wb_motor_set_position(left_motor, INFINITY);
    wb_motor_set_position(right_motor, INFINITY);
    
    motors_stop();
}
