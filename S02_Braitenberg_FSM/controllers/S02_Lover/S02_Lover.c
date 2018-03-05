/********************************************************************************************
Empty controller

Author: Julien Nembrini, Simon Studer (University of Fribourg)
Version: 1.0 (2016-02-25)
********************************************************************************************/
#include <stdlib.h>

#include <webots/robot.h>

#define TIME_STEP 32

int main() {
  wb_robot_init();
  
  while (wb_robot_step(TIME_STEP)!=-1) {
    
  }
  
  wb_robot_cleanup();
  return EXIT_SUCCESS;
