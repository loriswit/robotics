#ifndef S02_BRAITENBERG_FSM_SENSOR_H
#define S02_BRAITENBERG_FSM_SENSOR_H

#define SENSORS_COUNT 8

// initializes proximity sensors
void sensors_init();

// returns the current distance of a proximity sensor
double sensors_get_distance(size_t index, bool correct);

#endif //S02_BRAITENBERG_FSM_SENSOR_H
