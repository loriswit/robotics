#include <webots/robot.h>
#include <webots/distance_sensor.h>

#include <stdlib.h>

#include "sensors.h"
#include "leds.h"
#include "consts.h"

WbDeviceTag sensors[SENSORS_COUNT];
double corrections[SENSORS_COUNT] = {0, 0};

#define CALIB_COUNT 50
#define CALIB_OFFSET 5

// computes calibration values for proximity sensors
void sensors_compute_corrections()
{
    for(unsigned i = 0; i < CALIB_COUNT + CALIB_OFFSET && wb_robot_step(TIME_STEP) != -1; ++i)
    {
        leds_flash();
        
        if(i >= CALIB_OFFSET)
            for(size_t j = 0; j < SENSORS_COUNT; ++j)
                corrections[j] += wb_distance_sensor_get_value(sensors[j]);
    }
    
    for(size_t i = 0; i < SENSORS_COUNT; ++i)
        corrections[i] /= CALIB_COUNT;
    
    leds_set(false);
}

void sensors_init()
{
    static const char * prox_sensors_names[SENSORS_COUNT] =
            {"ps0", "ps1", "ps2", "ps3", "ps4", "ps5", "ps6", "ps7"};
    
    for(size_t i = 0; i < SENSORS_COUNT; ++i)
    {
        sensors[i] = wb_robot_get_device(prox_sensors_names[i]);
        wb_distance_sensor_enable(sensors[i], TIME_STEP);
    }
    
    sensors_compute_corrections();
}

double sensors_get_distance(size_t index, bool correct)
{
    double sensor_value = wb_distance_sensor_get_value(sensors[index]);
    return correct ? sensor_value - corrections[index] : sensor_value;
}
