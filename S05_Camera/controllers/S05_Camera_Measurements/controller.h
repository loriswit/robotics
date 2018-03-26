#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

//#include <webots/differential_wheels.h>
#include <webots/robot.h>


#define TIME_STEP 32


#define NONE  -1
#define RED    0
#define GREEN  1
#define BLUE   2
#define ORANGE 3

// basic movement
#define NORM_SPEED   2.0
#define THRESH_PROX  400.0

// motors
void motors_init();
void set_speed(double,double);
void turn_left(double);
void turn_right(double);
void stop();


// proximity
void prox_init();
void prox_update();
double prox_get_left_weighted(int,int,int,int);
double prox_get_right_weighted(int,int,int,int);
//double prox_get_left_front();
//double prox_get_right_front();


// ground sensor

#define TH_GS_WHITE 500

void ground_init();
void ground_update();
double ground_get_weighted(int,int,int);
int ground_online();

// camera
void cam_init();
void cam_start();
void cam_stop();
void cam_update();
int cam_see_color(int);
int cam_see_color_position(int);

// led
void led_init();
void led_set(int,int);
void led_all_off();
void led_all_on();


// miscellaneous
int signum(double);


#endif

