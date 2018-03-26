#include "controller.h"

#include <webots/motor.h>
#include <webots/distance_sensor.h>
#include <webots/camera.h>
#include <webots/receiver.h>
#include <webots/emitter.h>
#include <webots/led.h>

int signum(double x) {
  return (x > 0) ? 1 : ((x < 0) ? -1 : 0);
}



///////////////////////

WbDeviceTag left_motor, right_motor;

#define MAX_SPEED 6.0

void motors_init() {

  // get a handler to the motors 
  left_motor = wb_robot_get_device("left wheel motor");
  right_motor = wb_robot_get_device("right wheel motor");
  
  // set target position to infinity (speed control)
  wb_motor_set_position(left_motor, INFINITY);
  wb_motor_set_position(right_motor, INFINITY);
  
  // set speed to 0
  wb_motor_set_velocity(left_motor, 0.0);
  wb_motor_set_velocity(right_motor, 0.0);

}

// constrain speed to ± MAX_SPEED
double bounded_speed(double speed) { 
  if (speed > MAX_SPEED) return MAX_SPEED;
  else if (speed < -MAX_SPEED) return -MAX_SPEED;
  else return speed;
}

void set_speed(double left, double right) {
  wb_motor_set_velocity(left_motor, bounded_speed(left));
  wb_motor_set_velocity(right_motor,bounded_speed(right));
}


void turn_left(double speed) {
  wb_motor_set_velocity(left_motor, -bounded_speed(speed));
  wb_motor_set_velocity(right_motor, bounded_speed(speed));
}

void turn_right(double speed) {
  wb_motor_set_velocity(left_motor,  bounded_speed(speed));
  wb_motor_set_velocity(right_motor,-bounded_speed(speed));
}

void stop() {
  wb_motor_set_velocity(left_motor, 0);
  wb_motor_set_velocity(right_motor,0);
}


///////////////////////



#define PROX_SENSORS_NUMBER 8
#define WALL_THRES 500
#define TURN_SPEED 2.0

static const char *prox_sensors_names[PROX_SENSORS_NUMBER] =
  {"ps0", "ps1", "ps2", "ps3", "ps4", "ps5", "ps6", "ps7"};
static WbDeviceTag prox_sensor_tags[PROX_SENSORS_NUMBER];
static double prox_corr_vals[PROX_SENSORS_NUMBER];
static double prox[PROX_SENSORS_NUMBER];

#define NBR_CALIB 50
#define OFFSET_CALIB 5


// get the correction values for prox sensors
void get_prox_corr_vals() {
  int i, j;
 
  
  // init array for calibration values  
  for (i=0; i<PROX_SENSORS_NUMBER; i++) {
    prox_corr_vals[i] = 0;
  }

  // get multiple readings for each sensor
  for (j=0; j<(NBR_CALIB+OFFSET_CALIB) && wb_robot_step(TIME_STEP)!=-1; j++) {
    for (i=0; i<PROX_SENSORS_NUMBER; i++) {
      if (j>=OFFSET_CALIB) 
        prox_corr_vals[i] += wb_distance_sensor_get_value(prox_sensor_tags[i]);
    }
  }
  
  // calculate average for each sensor
  for (i=0; i<PROX_SENSORS_NUMBER; i++) {
    prox_corr_vals[i] = prox_corr_vals[i] / NBR_CALIB;
  }
  
}

void prox_update() {
  int i;
  // fetch prox sensor values and calibrate
  for (i=0; i<PROX_SENSORS_NUMBER; i++) {
    prox[i] = wb_distance_sensor_get_value(prox_sensor_tags[i])-prox_corr_vals[i];
  }
}

double prox_get_left_weighted(int a,int b,int c,int d) {
    return (a*prox[7] + b*prox[6] + c*prox[5] + d*prox[5])/(a+b+c+d);
}

double prox_get_right_weighted(int a,int b,int c,int d) {
    return (a*prox[0] + b*prox[1] + c*prox[2] + d*prox[3])/(a+b+c+d);
}


void prox_init() {
  // init prox sensors
  int i;
  for (i=0;  i<PROX_SENSORS_NUMBER; i++) {
    prox_sensor_tags[i] = wb_robot_get_device(prox_sensors_names[i]);
    wb_distance_sensor_enable(prox_sensor_tags[i], 4*TIME_STEP);
  }
  get_prox_corr_vals();
}


//////////////////////////////////////


// 3 IR ground color sensors
#define GROUND_SENSORS_NUMBER 3
#define GS_LEFT 0
#define GS_CENTER 1
#define GS_RIGHT 2

const char *ground_sensors_names[GROUND_SENSORS_NUMBER] =
  {"gs0", "gs1", "gs2"};
WbDeviceTag gs[GROUND_SENSORS_NUMBER]; /* ground sensors */
unsigned short gs_value[GROUND_SENSORS_NUMBER]={0,0,0};

void ground_init() {
  int i;
  for (i = 0; i < GROUND_SENSORS_NUMBER; i++) {
    gs[i] = wb_robot_get_device(ground_sensors_names[i]); /* ground sensors */
    wb_distance_sensor_enable(gs[i],TIME_STEP);
  }
}


void ground_update() {
    int i;
    for(i=0;i<GROUND_SENSORS_NUMBER;i++) {
      gs_value[i] = wb_distance_sensor_get_value(gs[i]);
    }
}

double ground_get_weighted(int a,int b, int c) {
    return (a*gs_value[GS_LEFT] + b*gs_value[GS_CENTER] + c*gs_value[GS_RIGHT] )/(a+b+c);
}






////////////////////////////////////


#define LED_NUMBER 8

static const char *led_names[LED_NUMBER] =
  {"led0", "led1", "led2", "led3", "led4", "led5", "led6", "led7"};
static WbDeviceTag led_tags[LED_NUMBER];


void led_init() {
  int i;
  for (i=0;  i<LED_NUMBER; i++) {
    led_tags[i] = wb_robot_get_device(led_names[i]);
    wb_led_set(led_tags[i], 0);
  }
}

void led_set(int n, int on) {
    wb_led_set(led_tags[n], on);
}

void led_all_off() {
  int i;
  for (i = 0; i < LED_NUMBER; i++) {
    wb_led_set(led_tags[i], 0);
  }
}

void led_all_on() {
  int i;
  for (i = 0; i < LED_NUMBER; i++) {
    wb_led_set(led_tags[i], 1);
  }
}





