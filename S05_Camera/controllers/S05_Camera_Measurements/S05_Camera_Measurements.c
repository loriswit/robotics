/********************************************************************************************
Camera Measurements

Description: Writes camera data to CSV files to create histogram.
Author: Beat Hirsbrunner, Julien Nembrini, Simon Studer (University of Fribourg)
Version: 1.0 (2016-03-24)
********************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "controller.h"

#include <webots/robot.h>
#include <webots/camera.h>


#define MAX_SAMPLES 15
#define WAIT_STEPS 10
#define CAM_RATE 8

WbDeviceTag cam;

int main(int argc, char **argv) {
  const unsigned char* im;
  long int gr, r, g, b;
  int camera_width, camera_height;
  int i, m, n;
  
  
  wb_robot_init();
  motors_init();
  
  cam = wb_robot_get_device("camera");
  wb_camera_enable(cam,CAM_RATE*TIME_STEP);
  camera_width = wb_camera_get_width(cam);
  camera_height = wb_camera_get_height(cam);

  // open files for writing
  FILE *csvGR = fopen("cameraValuesGray.csv", "w");
  FILE *csvR = fopen("cameraValuesRed.csv", "w");
  FILE *csvG = fopen("cameraValuesGreen.csv", "w");
  FILE *csvB = fopen("cameraValuesBlue.csv", "w");
  
  // check if files opened correctly
  if ((csvGR == NULL) || (csvR == NULL) || (csvG == NULL) || (csvB == NULL)){
    printf("Error opening csv file!\n");
    exit(1);
  }
  
  // write header of files
  fprintf(csvGR, "line,");
  fprintf(csvR, "line,");
  fprintf(csvG, "line,");
  fprintf(csvB, "line,");


  for (i=0; i<camera_width; i++) {
    fprintf(csvGR, "p%d,", i);
    fprintf(csvR, "p%d,", i);
    fprintf(csvG, "p%d,", i);
    fprintf(csvB, "p%d,", i);
  }

  fprintf(csvGR, "\n");
  fprintf(csvR, "\n");
  fprintf(csvG, "\n");
  fprintf(csvB, "\n");

  for (i=0; i<WAIT_STEPS+CAM_RATE*MAX_SAMPLES && wb_robot_step(TIME_STEP)!=-1; i++) {
    if ((i > WAIT_STEPS) && (((i-WAIT_STEPS)%CAM_RATE)==0)) {

      //set_speed(-2.0,-2.0);
      set_speed(0.0,0.0);
    
        im = wb_camera_get_image(cam);
        for (n=0; n<camera_height; n++) {
        
          fprintf(csvGR, "%d,", n);
          fprintf(csvR, "%d,", n);
          fprintf(csvG, "%d,", n);
          fprintf(csvB, "%d,", n);
          
          for (m=0;m<camera_width;m++) {
            gr = wb_camera_image_get_grey(im, camera_width, m, n);
            r = wb_camera_image_get_red(im, camera_width, m, n);
            g = wb_camera_image_get_green(im, camera_width, m, n);
            b = wb_camera_image_get_blue(im, camera_width, m, n);
            
            fprintf(csvGR, "%ld,", gr);
            fprintf(csvR, "%ld,", r);
            fprintf(csvG, "%ld,", g);
            fprintf(csvB, "%ld,", b);
          }
          
          fprintf(csvGR, "\n");
          fprintf(csvR, "\n");
          fprintf(csvG, "\n");
          fprintf(csvB, "\n");
        } 
        printf(" camera sample %d\n", i-WAIT_STEPS);        

    }

    printf("%d\n", i-WAIT_STEPS);        
    
    
  };

  set_speed(0.0,0.0);


  // close files
  fclose(csvGR);
  fclose(csvR);
  fclose(csvG);
  fclose(csvB);
  
  wb_robot_cleanup();
  
  return EXIT_SUCCESS;
}
