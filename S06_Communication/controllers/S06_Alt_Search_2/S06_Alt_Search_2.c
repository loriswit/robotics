#include <stdio.h>
#include <stdarg.h>

#include <webots/robot.h>

#include "../util/com.h"
#include "../util/motors.h"
#include "../util/leds.h"
#include "../util/prox_sensors.h"
#include "../util/consts.h"

#define THRESHOLD 400
#define SPEED 4
#define MAX_SPEED 6
#define LOVER_DISTANCE_THRESHOLD 1700
#define EXPLORER_DISTANCE_THRESHOLD 50
#define MIN_STATE_DURATION 2.5 // in seconds

typedef enum
{
    LOVER, EXPLORER, STOP, NO_DATA
} state;

// returns the sum of a array
double array_sum(const double array[], size_t length)
{
    double sum = 0;
    for(size_t i = 0; i < length; ++i)
        sum += array[i];
    
    return sum;
}

void println(const char * format, ...)
{
    va_list args;
    va_start(args, format);
    
    char message[100];
    vsprintf(message, format, args);
    
    printf("%s: %s\n", wb_robot_get_name(), message);
}

void send(int value)
{
    packet message;
    message.data = (int[]) {value};
    message.size = sizeof(value);
    
    com_send(message);
}

int receive()
{
    packet message = com_receive();
    if(message.data == NULL)
        return NO_DATA;
    
    int value = *(int *) message.data;
    free(message.data);
    
    return value;
}

int main(int argc, char ** argv)
{
    wb_robot_init();
    com_init();
    motors_init();
    leds_init();
    prox_init();
    
    static const double lover_weights[PROX_COUNT] =
            {2, 2, 3, 4, 4, 3, 2, 2};
    static const double explorer_weights[PROX_COUNT] =
            {4, 3, 2, 1, 1, 2, 3, 4};
    
    const double lover_total_weight = array_sum(lover_weights, PROX_COUNT);
    const double explorer_total_weight = array_sum(explorer_weights, PROX_COUNT);
    
    const double time_factor = wb_robot_get_mode() == 0 ? 1 : 0.4;
    println("time factor = %f", time_factor);
    
    const unsigned max_counter = (unsigned) (MIN_STATE_DURATION * 1000 / TIME_STEP * time_factor);
    unsigned counter = max_counter;
    
    state robot_state = LOVER;
    
    while(wb_robot_step(TIME_STEP) != -1)
    {
        double distance = (prox_get_value(0, true) + prox_get_value(7, true)) / 2;
        state other_state = (state) receive();
        
        if(++counter >= max_counter)
        {
            if(robot_state == LOVER && distance > LOVER_DISTANCE_THRESHOLD)
            {
                robot_state = STOP;
                leds_set(false);
                motors_stop();
                
                send(robot_state);
                println("stopping");
            }
            
            if(robot_state == STOP && other_state == STOP)
            {
                robot_state = EXPLORER;
                counter = 0;
                println("switching to explorer");
            }
            
            else if(robot_state == EXPLORER && distance < EXPLORER_DISTANCE_THRESHOLD)
            {
                robot_state = LOVER;
                counter = max_counter * 3 / 4; // shorter timer
                println("switching to lover");
            }
        }
        
        switch(robot_state)
        {
            case LOVER:
            case EXPLORER:
                leds_set(robot_state == LOVER);
                break;
            
            case STOP:
                leds_spin();
                continue;
            
            case NO_DATA:
                return EXIT_FAILURE;
        }
        
        double prox[2] = {0, 0};
        for(size_t i = 0; i < PROX_COUNT; ++i)
        {
            double weight = robot_state == LOVER ? lover_weights[i] : explorer_weights[i];
            double total_weight = robot_state == LOVER ? lover_total_weight : explorer_total_weight;
            prox[i * 2 / PROX_COUNT] +=
                    weight * prox_get_value(i, true) / total_weight;
        }
        
        double speed_right = SPEED - prox[robot_state == LOVER ? 0 : 1] / THRESHOLD * SPEED;
        double speed_left = SPEED - prox[robot_state == LOVER ? 1 : 0] / THRESHOLD * SPEED;
        
        motors_set_speed(speed_left, speed_right);
    }
    
    leds_set(false);
    
    wb_robot_cleanup();
    
    return EXIT_SUCCESS;
}
