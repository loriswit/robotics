#ifndef ROBOTICS_MOTORS_H
#define ROBOTICS_MOTORS_H

/**
 * Initializes both motors.
 */
void motors_init();

/**
 * Sets both motors individual speed.
 *
 * @param left The speed of the left motor (in rad/s)
 * @param right The speed of the right motor (in rad/s)
 */
void motors_set_speed(double left, double right);

/**
 * Sets both motors speed to zero.
 */
void motors_stop();

#endif //ROBOTICS_MOTORS_H
