#include <stdlib.h>
#include <stdio.h>

#include <webots/robot.h>

#include "../util/ground_sensors.h"
#include "../util/motors.h"
#include "../util/consts.h"

#define STEPS_COUNT 50
#define SPEED 2

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
    
    for(unsigned i = 0; i < GROUND_COUNT; ++i)
        fprintf(file, "ground sensor %u,", i);
    
    fprintf(file, "\n");
    
    return file;
}

// stores current time and sensors values in a new row
void csv_add_row(FILE * file, double time)
{
    fprintf(file, "%f,", time);
    
    for(size_t i = 0; i < GROUND_COUNT; ++i)
        fprintf(file, "%f,", ground_get_value(i));
    
    fprintf(file, "\n");
}


int main(int argc, char ** argv)
{
    wb_robot_init();
    motors_init();
    ground_init();
    
    FILE * file = csv_create("gs.csv");
    
    if(file == NULL)
    {
        printf("Cannot open file for writing.\n");
        return EXIT_FAILURE;
    }
    
    for(unsigned n = 0; n < STEPS_COUNT && wb_robot_step(TIME_STEP) != -1; n++)
    {
        csv_add_row(file, wb_robot_get_time());
        motors_set_speed(SPEED, SPEED);
    }
    
    fclose(file);
    
    motors_stop();
    wb_robot_cleanup();
    
    return EXIT_SUCCESS;
}
