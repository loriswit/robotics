#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <webots/robot.h>

#include "../util/motors.h"
#include "../util/leds.h"
#include "../util/prox_sensors.h"
#include "../util/consts.h"

#define SPEED 2

// General parameters
#define PID_WALL_FOLLOW_TARGET 700
#define PID_PROX_THRESHOLD 50
#define PID_NORM_SPEED_WALL 3
#define PID_COUNTER 10
#define WEIGHT_FRONT 3
#define WEIGHT_FRONT_SIDE 1
#define WEIGHT_SIDE 1
#define WEIGHT_BACK 0

#define WEIGHTS_COUNT 4
const double WEIGHTS[WEIGHTS_COUNT] = {WEIGHT_FRONT, WEIGHT_FRONT_SIDE, WEIGHT_SIDE, WEIGHT_BACK};

// PID parameters
//TODO
#define K 0.0016
#define T_I 9
#define T_D 0.35

// states
#define FORWARD 0
#define DO_PID  1

double error = 0;
double deriv = 0;
double integ = 0;

double calculate_pid(double proxLR)
{
    double prev_err = error;
    
    // Very basic error calculation. Feel free to improve it.
    error = proxLR - PID_WALL_FOLLOW_TARGET;
    deriv = (error - prev_err) * 1000 / TIME_STEP;
    integ += error * TIME_STEP / 1000;
    
    return K * error + K * T_D * deriv + K / T_I * integ;
}

// creates a CSV file with headers
FILE * csv_create(const char * filename)
{
    FILE * file = fopen(filename, "w");
    if(file == NULL)
    {
        printf("Cannot open file: %s\n", filename);
        return NULL;
    }
    
    // print headers
    fprintf(file, "time,P,I,D,ds,left speed,right speed\n");
    
    return file;
}

// stores current time and sensors values in a new row
void csv_add_row(FILE * file, double ds, double speed_left, double speed_right)
{
    fprintf(file, "%f,%f,%f,%f,%f,%f,%f\n", wb_robot_get_time(), K * error, K * integ / T_I, K * deriv * T_D, ds,
            speed_left, speed_right);
}

// returns the sum of a array
double array_sum(const double array[], size_t length)
{
    double sum = 0;
    for(size_t i = 0; i < length; ++i)
        sum += array[i];
    
    return sum;
}

int main(int argc, char ** argv)
{
    wb_robot_init();
    motors_init();
    leds_init();
    prox_init();
    
    int state = FORWARD;
    int counter = 0;
    
    FILE * log = csv_create("logPID.csv");
    if(log == NULL)
        return EXIT_FAILURE;
    
    while(wb_robot_step(TIME_STEP) != -1)
    {
        leds_set(false);
        
        double prox_right = 0;
        for(size_t i = 0; i < WEIGHTS_COUNT; ++i)
            prox_right += WEIGHTS[i] * prox_get_value(i, true);
        prox_right /= array_sum(WEIGHTS, WEIGHTS_COUNT);
        
        // states
        if((prox_right < PID_PROX_THRESHOLD) /*&& (prox_left < PID_PROX_THRESHOLD)*/)
        {
            if(counter++ > PID_COUNTER)
            {
                state = FORWARD;
                counter = 0;
            }
        }
        else
        {
            state = DO_PID;
            counter = 0;
        }
    
        double speed_left = 0;
        double speed_right = 0;
        
        if(state == FORWARD)
        {
            speed_right = SPEED;
            speed_left = SPEED;
        }
        else
        {
            if(prox_right > PID_PROX_THRESHOLD /*prox_left*/)
            {
                //compute PID response according to IR sensor value
                double ds = calculate_pid(prox_right);
                
                // adjust speed values to turn away from obstacle
                speed_right = (fabs(ds) > 1 ? 0 : PID_NORM_SPEED_WALL) + ds;
                speed_left = (fabs(ds) > 1 ? 0 : PID_NORM_SPEED_WALL) - ds;
                
                led_set(1, true);
                led_set(2, true);
                led_set(3, true);
    
                csv_add_row(log, ds, speed_left, speed_right);
            }
            else
            {
                //implement left side if desired
            }
        }
        
        motors_set_speed(speed_left, speed_right);
    }
    
    fclose(log);
    wb_robot_cleanup();
    
    return EXIT_SUCCESS;
}
