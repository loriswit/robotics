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

#define STOP_FLAG 1

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
    
    enum
    {
        LOVER, EXPLORER, STOP
    } state = LOVER;
    
    while(wb_robot_step(TIME_STEP) != -1)
    {
        double distance =
                (prox_get_value(0, true) + prox_get_value(7, true)) / 2;
        
        packet message = com_receive();
        
        if(++counter >= max_counter)
        {
            if(state == LOVER && distance > LOVER_DISTANCE_THRESHOLD)
            {
                state = STOP;
                motors_stop();
                
                char data[1] = {STOP_FLAG};
                com_send((packet) {data, sizeof(data)});
                
                println("stopping");
            }
            
            if(state == STOP)
            {
                leds_spin();
                if(message.data != NULL && message.data[0] == STOP_FLAG)
                {
                    state = EXPLORER;
                    counter = 0;
                    println("switching to explorer");
                }
            }
            
            else if(state == EXPLORER && distance < EXPLORER_DISTANCE_THRESHOLD)
            {
                state = LOVER;
                counter = max_counter * 3 / 4; // shorter timer
                println("switching to lover");
            }
        }
        
        if(state == STOP)
            continue;
        
        double prox[2] = {0, 0};
        for(size_t i = 0; i < PROX_COUNT; ++i)
        {
            double weight = state == LOVER ? lover_weights[i] : explorer_weights[i];
            double total_weight = state == LOVER ? lover_total_weight : explorer_total_weight;
            prox[i * 2 / PROX_COUNT] +=
                    weight * prox_get_value(i, true) / total_weight;
        }
        
        double speed_right = SPEED - prox[state == LOVER ? 0 : 1] / THRESHOLD * SPEED;
        double speed_left = SPEED - prox[state == LOVER ? 1 : 0] / THRESHOLD * SPEED;
        
        motors_set_speed(speed_left, speed_right);
        
        leds_set(state == LOVER);
    }
    
    leds_set(false);
    
    wb_robot_cleanup();
    
    return EXIT_SUCCESS;
}
