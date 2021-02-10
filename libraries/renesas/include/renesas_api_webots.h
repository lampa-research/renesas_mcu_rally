#ifndef RENESAS_API_WEBOTS_H_
#define RENESAS_API_WEBOTS_H_

// includes
#include <stdio.h>
#include <webots/robot.h>
#include <webots/supervisor.h>
#include <webots/motor.h>
#include <webots/distance_sensor.h>
#include <webots/light_sensor.h>
#include <webots/position_sensor.h>

// motor names
#define BR_MOTOR_NAME "back_right"
#define BL_MOTOR_NAME "back_left"
#define FR_MOTOR_NAME "front_right"
#define FL_MOTOR_NAME "front_left"

// defines and macros
#define TIME_STEP 5
#define constrain(amt, low, high) ((amt) < (low) ? (low) : ((amt) > (high) ? (high) : (amt)))

// function prototypes
float compute_torque(float u, float om, float gear);
double get_gear_ratio();
int get_number_sensors();

#endif
