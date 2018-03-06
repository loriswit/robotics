#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include <webots/robot.h>

#include "../util/motors.h"
#include "../util/leds.h"
#include "../util/sensors.h"
#include "../util/consts.h"

// in seconds
#define WAIT_DURATION 3
#define MOVE_DURATION 5

#define WAIT_FOR_DISTANCE 2000

// only use proximity sensor 0 and 7
#define USED_SENSORS_COUNT 2
const size_t USED_SENSORS[USED_SENSORS_COUNT] = {0, 7};

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
    fprintf(file, "time,");
    
    for(size_t i = 0; i < USED_SENSORS_COUNT; ++i)
        fprintf(file, "proximity sensor %u,", (unsigned) USED_SENSORS[i]);
    
    fprintf(file, "\n");
    
    return file;
}

// stores current time and sensors values in a new row
void csv_add_row(FILE * file, double time, bool correct)
{
    fprintf(file, "%f,", time);
    
    for(size_t i = 0; i < USED_SENSORS_COUNT; ++i)
    {
        double sensor_value = sensors_get_value(USED_SENSORS[i], correct);
        fprintf(file, "%f,", sensor_value);
    }
    
    fprintf(file, "\n");
}

// wait until the robot has been manually placed close to a wall
void wait_for_wall()
{
    printf("Please place the robot close to a wall…\n");
    
    const static unsigned init_countdown = WAIT_DURATION * 1000 / TIME_STEP;
    unsigned countdown = init_countdown;
    
    leds_set(true);
    
    while(wb_robot_step(TIME_STEP) != -1)
    {
        for(size_t i = 0; i < USED_SENSORS_COUNT; ++i)
            if(sensors_get_value(USED_SENSORS[i], true) < WAIT_FOR_DISTANCE)
                countdown = init_countdown;
        
        if(--countdown == 0)
            break;
    }
    
    leds_set(false);
}

int main()
{
    wb_robot_init();
    motors_init();
    leds_init();
    sensors_init();
    
    wait_for_wall();
    
    FILE * calib = csv_create("calib.csv");
    FILE * uncalib = csv_create("uncalib.csv");
    
    if(calib == NULL || uncalib == NULL)
        return EXIT_FAILURE;
    
    const double speed = -2;
    const double start_time = wb_robot_get_time();
    
    while(wb_robot_step(TIME_STEP) != -1)
    {
        motors_set_speed(speed, speed);
        
        double current_time = wb_robot_get_time() - start_time;
        
        csv_add_row(calib, current_time, true);
        csv_add_row(uncalib, current_time, false);
        
        if(current_time >= MOVE_DURATION)
            break;
    }
    
    fclose(calib);
    fclose(uncalib);
    
    motors_stop();
    
    wb_robot_cleanup();
    return EXIT_SUCCESS;
}
