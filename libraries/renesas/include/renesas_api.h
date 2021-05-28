#ifndef RENESAS_API_H_
#define RENESAS_API_H_

// includes
#include <stdio.h>
#include <webots/robot.h>
#include <webots/supervisor.h>
#include <webots/motor.h>
#include <webots/distance_sensor.h>
#include <webots/light_sensor.h>
#include <webots/position_sensor.h>
#include <webots/inertial_unit.h>

#include "renesas_api_webots.h"

// high-level function prototypes
void init();
void update();
void handle(int angle);
void motor(int back_right, int back_left, int front_right, int front_left);
unsigned short *line_sensor();
double *encoders();
double *imu();
double time();

double get_track();
double get_wheel_base();
double get_sensor_base();
double get_gear_ratio();
const char* get_tyre_type();
double get_tyre_width();
double get_tyre_radius();
int get_number_of_sensors();
double get_distance_between_sensors();
double get_weight_penalty();

#endif
